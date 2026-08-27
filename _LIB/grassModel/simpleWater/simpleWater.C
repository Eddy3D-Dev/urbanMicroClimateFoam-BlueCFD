#include "simpleWater.H"
#include "addToRunTimeSelectionTable.H"
#include "constants.H"

using namespace Foam::constant;

namespace Foam
{
namespace grass
{
    defineTypeNameAndDebug(simpleWater, 0);
    addToGrassRunTimeSelectionTables(simpleWater);
}
}

void Foam::grass::simpleWater::initialise()
{
    albedo_ = coeffs_.lookupOrDefault<scalar>("albedo", 0.08);
    emissivity_ = coeffs_.lookupOrDefault<scalar>("emissivity", 0.97);
    depth_ = coeffs_.lookupOrDefault<scalar>("depth", 0.5);
    heatCapacity_ = coeffs_.lookupOrDefault<scalar>("heatCapacity", 4182.0);
    density_ = coeffs_.lookupOrDefault<scalar>("density", 998.0);
    aerodynamicResistance_ = coeffs_.lookupOrDefault<scalar>("aerodynamicResistance", -1.0);
    p_ = coeffs_.lookupOrDefault<scalar>("pressure", 101325.0);
    rhoAir_ = coeffs_.lookupOrDefault<scalar>("airDensity", 1.225);
    cpAir_ = coeffs_.lookupOrDefault<scalar>("airHeatCapacity", 1003.5);
    relax_ = coeffs_.lookupOrDefault<scalar>("relaxation", 0.35);
    minTemperature_ = coeffs_.lookupOrDefault<scalar>("minTemperature", 273.15);
    maxTemperature_ = coeffs_.lookupOrDefault<scalar>("maxTemperature", 333.15);
    // Air boundary normals point out of the air and into the water. The free
    // surface therefore points down; side and submerged faces are excluded.
    maxNormalZ_ = coeffs_.lookupOrDefault<scalar>("maxNormalZ", -0.5);
    Ra_ = 287.042;
    Rv_ = 461.524;

    const wordList patches(coeffs_.lookup("waterPatches"));
    label count = 0;
    forAll(patches, i)
    {
        const label patchId = mesh_.boundaryMesh().findIndex(patches[i]);
        if (patchId < 0)
        {
            FatalErrorInFunction << "Water patch named " << patches[i]
                << " not found." << nl << abort(FatalError);
        }
        selectedPatches_[count++] = patchId;
    }
    selectedPatches_.resize(count);
}

Foam::scalarField Foam::grass::simpleWater::saturationPressure
(
    const scalarField& temperature
) const
{
    return exp
    (
        -5.8002206e3/temperature + 1.3914993
      - 4.8640239e-2*temperature + 4.1764768e-5*pow(temperature, 2)
      - 1.4452093e-8*pow(temperature, 3) + 6.5459673*log(temperature)
    );
}

Foam::grass::simpleWater::simpleWater(const volScalarField& T)
:
    grassModel(typeName, T),
    Tw_
    (
        IOobject("Tw", mesh_.time().name(), mesh_, IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE),
        T
    ),
    Sw_
    (
        IOobject("waterSw", mesh_.time().name(), mesh_, IOobject::NO_READ, IOobject::NO_WRITE),
        mesh_, dimensionedScalar("zero", dimensionSet(1,-3,-1,0,0,0,0), 0.0)
    ),
    Sh_
    (
        IOobject("waterSh", mesh_.time().name(), mesh_, IOobject::NO_READ, IOobject::NO_WRITE),
        mesh_, dimensionedScalar("zero", dimensionSet(1,-1,-3,0,0,0,0), 0.0)
    ),
    Cf_
    (
        IOobject("waterCf", mesh_.time().name(), mesh_, IOobject::NO_READ, IOobject::NO_WRITE),
        mesh_, dimensionedScalar("zero", dimensionSet(0,-1,0,0,0,0,0), 0.0)
    ),
    selectedPatches_(mesh_.boundary().size(), -1)
{
    initialise();
}

Foam::grass::simpleWater::~simpleWater() {}

bool Foam::grass::simpleWater::read()
{
    if (!grassModel::read()) return false;
    initialise();
    return true;
}

void Foam::grass::simpleWater::calculate
(
    const volScalarField& T,
    const volScalarField& w,
    const volVectorField& U
)
{
    Sh_ = dimensionedScalar("zero", Sh_.dimensions(), 0.0);
    Sw_ = dimensionedScalar("zero", Sw_.dimensions(), 0.0);

    const scalar latentHeat = 2.5e6;
    const scalar dt = max(time_.deltaTValue(), SMALL);

    forAll(selectedPatches_, patchi)
    {
        const label patchId = selectedPatches_[patchi];
        const fvPatch& patch = mesh_.boundary()[patchId];
        const labelUList& cells = patch.faceCells();
        const scalarField airT = patch.patchInternalField(T);
        const scalarField airW = patch.patchInternalField(w);
        const vectorField airU = patch.patchInternalField(U);
        const scalarField qs = patch.lookupPatchField<volScalarField, scalar>("qs");
        const scalarField qr = patch.lookupPatchField<volScalarField, scalar>("qr");
        const scalarField delta = patch.deltaCoeffs();
        const vectorField normals = patch.nf();
        scalarField waterT = patch.patchInternalField(Tw_);

        scalarField ra(patch.size(), aerodynamicResistance_);
        if (aerodynamicResistance_ < 0)
        {
            // Neutral bulk transfer approximation; bounded at low wind speed.
            ra = 100.0/max(mag(airU), scalarField(patch.size(), 0.2));
        }

        const scalarField h = rhoAir_*cpAir_/ra;
        const scalarField hm = rhoAir_*Ra_/(p_*Rv_*ra);
        const scalarField pvAir = p_*airW/(Ra_/Rv_ + airW);
        const scalarField pvSat = saturationPressure(waterT);
        const scalarField evaporation = max(hm*(pvSat - pvAir), scalarField(patch.size(), 0.0));
        const scalarField sensible = h*(waterT - airT);
        // qr is the solar-load model's signed long-wave balance at the patch,
        // not raw incident irradiance. Applying Stefan-Boltzmann here would count
        // the surface emission twice.
        const scalarField netLongwave = emissivity_*qr;
        const scalarField netShortwave = (1.0 - albedo_)*qs;
        const scalarField net = netShortwave + netLongwave - sensible - latentHeat*evaporation;
        const scalar storage = max(density_*heatCapacity_*depth_, SMALL);
        scalarField updated = waterT + dt*net/storage;
        updated = min(max(updated, minTemperature_), maxTemperature_);
        waterT = (1.0 - relax_)*waterT + relax_*updated;

        scalarField& twInternal = Tw_.primitiveFieldRef();
        scalarField& twPatch = Tw_.boundaryFieldRef()[patchId];
        forAll(cells, facei)
        {
            if (normals[facei].z() > maxNormalZ_) continue;
            twInternal[cells[facei]] = waterT[facei];
            twPatch[facei] = waterT[facei];
            // Convert the surface flux to an equivalent source in the adjacent cell.
            Sh_[cells[facei]] += sensible[facei]*delta[facei];
            Sw_[cells[facei]] += evaporation[facei]*delta[facei];
        }
    }
}

Foam::tmp<Foam::volScalarField> Foam::grass::simpleWater::Sh() const
{
    return tmp<volScalarField>
    (
        new volScalarField
        (
            IOobject("waterSh", mesh_.time().name(), mesh_, IOobject::NO_READ,
                IOobject::NO_WRITE, false),
            Sh_
        )
    );
}

Foam::tmp<Foam::volScalarField> Foam::grass::simpleWater::Cf() const
{
    return tmp<volScalarField>
    (
        new volScalarField
        (
            IOobject("waterCf", mesh_.time().name(), mesh_, IOobject::NO_READ,
                IOobject::NO_WRITE, false),
            Cf_
        )
    );
}

Foam::tmp<Foam::volScalarField> Foam::grass::simpleWater::Sw() const
{
    return tmp<volScalarField>
    (
        new volScalarField
        (
            IOobject("waterSw", mesh_.time().name(), mesh_, IOobject::NO_READ,
                IOobject::NO_WRITE, false),
            Sw_
        )
    );
}
