// File Automatically generated by eLiSe
#include "general/all.h"
#include "private/all.h"


class cEqAppui_NoDist__PTInc_M2CNoVar: public cElCompiledFonc
{
   public :

      cEqAppui_NoDist__PTInc_M2CNoVar();
      void ComputeVal();
      void ComputeValDeriv();
      void ComputeValDerivHessian();
      double * AdrVarLocFromString(const std::string &);
      void SetNDP0_x(double);
      void SetNDP0_y(double);
      void SetNDdx_x(double);
      void SetNDdx_y(double);
      void SetNDdy_x(double);
      void SetNDdy_y(double);
      void SetScNorm(double);
      void SetXIm(double);
      void SetYIm(double);


      static cAutoAddEntry  mTheAuto;
      static cElCompiledFonc *  Alloc();
   private :

      double mLocNDP0_x;
      double mLocNDP0_y;
      double mLocNDdx_x;
      double mLocNDdx_y;
      double mLocNDdy_x;
      double mLocNDdy_y;
      double mLocScNorm;
      double mLocXIm;
      double mLocYIm;
};
