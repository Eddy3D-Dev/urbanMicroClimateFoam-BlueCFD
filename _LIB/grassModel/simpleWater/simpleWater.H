#ifndef grassModelsimpleWater_H
#define grassModelsimpleWater_H

#include "grassModel.H"
#include "volFields.H"

namespace Foam
{
namespace grass
{

class simpleWater : public grassModel
{
    volScalarField Tw_;
    volScalarField Sw_;
    volScalarField Sh_;
    volScalarField Cf_;
    labelList selectedPatches_;

    scalar albedo_;
    scalar emissivity_;
    scalar depth_;
    scalar heatCapacity_;
    scalar density_;
    scalar aerodynamicResistance_;
    scalar p_;
    scalar rhoAir_;
    scalar cpAir_;
    scalar Ra_;
    scalar Rv_;
    scalar relax_;
    scalar minTemperature_;
    scalar maxTemperature_;
    scalar maxNormalZ_;

    void initialise();
    scalarField saturationPressure(const scalarField& temperature) const;

    simpleWater(const simpleWater&);
    void operator=(const simpleWater&);

public:
    TypeName("simpleWater");
    simpleWater(const volScalarField& T);
    virtual ~simpleWater();

    void calculate
    (
        const volScalarField& T,
        const volScalarField& w,
        const volVectorField& U
    );

    bool read();
    virtual tmp<volScalarField> Sh() const;
    virtual tmp<volScalarField> Cf() const;
    virtual tmp<volScalarField> Sw() const;
};

}
}

#endif
