#ifndef _FORMULA_GEOM3D_H_
#define _FORMULA_GEOM3D_H_

#include "SymbDer/SymbDer_Common.h"
#include "MMVII_Ptxd.h"
#include "MMVII_Stringifier.h"
#include "MMVII_DeclareCste.h"

#include "ComonHeaderSymb.h"

using namespace NS_SymbolicDerivative;


namespace MMVII
{

/**  Class for generating code relative to 3D-distance observation */
class cDist3D
{
  public :
    cDist3D() {}
    static const std::vector<std::string> VNamesUnknowns() { return {"x1","y1","z1","x2","y2","z2"}; }
    static const std::vector<std::string> VNamesObs()      { return {"D"}; }
    std::string FormulaName() const { return "Dist3D";}
    template <typename tUk,typename tObs>
             static std::vector<tUk> formula
                  (
                      const std::vector<tUk> & aVUk,
                      const std::vector<tObs> & aVObs
                  ) // const
    {
          const auto & x1 = aVUk[0];
          const auto & y1 = aVUk[1];
          const auto & z1 = aVUk[2];
          const auto & x2 = aVUk[3];
          const auto & y2 = aVUk[4];
          const auto & z2 = aVUk[5];

          const auto & ObsDist  = aVObs[0];
          return { sqrt(square(x1-x2) + square(y1-y2) + square(z1-z2)) - ObsDist } ;
     }
};

/**  Class for generating code relative equal 3D-distance observation */
class cDist3DParam
{
  public :
    cDist3DParam() {}
    static const std::vector<std::string> VNamesUnknowns() { return {"d","x1","y1","z1","x2","y2","z2"}; }
    static const std::vector<std::string> VNamesObs()      { return {}; }
    std::string FormulaName() const { return "Dist3DParam";}
    template <typename tUk,typename tObs>
             static std::vector<tUk> formula
                  (
                      const std::vector<tUk> & aVUk,
                      [[maybe_unused]] const std::vector<tObs> & aVObs
                  ) // const
    {
          const auto & d  = aVUk[0];
          const auto & x1 = aVUk[1];
          const auto & y1 = aVUk[2];
          const auto & z1 = aVUk[3];
          const auto & x2 = aVUk[4];
          const auto & y2 = aVUk[5];
          const auto & z2 = aVUk[6];

          return { sqrt(square(x1-x2) + square(y1-y2) + square(z1-z2)) - d } ;
     }
};



};//  namespace MMVII

#endif // _FORMULA_GEOM3D_H_
