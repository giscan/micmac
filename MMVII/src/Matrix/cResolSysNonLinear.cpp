#include "include/MMVII_all.h"
#include "include/MMVII_Tpl_Images.h"


using namespace NS_SymbolicDerivative;
using namespace MMVII;

namespace MMVII
{

/* ************************************************************ */
/*                                                              */
/*                cInputOutputRSNL                              */
/*                                                              */
/* ************************************************************ */

template <class Type>  cInputOutputRSNL<Type>::cInputOutputRSNL(const tVectInd& aVInd,const tStdVect & aVObs):
     mVInd  (aVInd),
     mObs   (aVObs)
{
}

template <class Type>  cInputOutputRSNL<Type>::cInputOutputRSNL(const tVectInd& aVInd,const tStdVect & aVTmp,const tStdVect & aVObs):
	cInputOutputRSNL<Type>(aVInd,aVObs)
{
	mTmpUK = aVTmp;
}

template <class Type> Type cInputOutputRSNL<Type>::WeightOfKthResisual(int aK) const
{
   switch (mWeights.size())
   {
	   case 0 :  return 1.0;
	   case 1 :  return mWeights[0];
	   default  : return mWeights.at(aK);
   }
}
template <class Type> size_t cInputOutputRSNL<Type>::NbUkTot() const
{
	return mVInd.size() + mTmpUK.size();
}

template <class Type> bool cInputOutputRSNL<Type>::IsOk() const
{
     if (mVals.size() !=mDers.size()) 
        return false;

     if (mVals.empty())
        return false;

     {
         size_t aNbUk = NbUkTot();
         for (const auto & aDer : mDers)
             if (aDer.size() != aNbUk)
                return false;
     }

     {
         size_t aSzW =  mWeights.size();
         if ((aSzW>1) && (aSzW!= mVals.size()))
            return false;
     }
     return true;
}


/* ************************************************************ */
/*                                                              */
/*                cSetIORSNL_SameTmp                            */
/*                                                              */
/* ************************************************************ */

template <class Type> cSetIORSNL_SameTmp<Type>::cSetIORSNL_SameTmp() :
	mOk (false),
	mNbEq (0)
{
}

template <class Type> void cSetIORSNL_SameTmp<Type>::AddOneEq(const tIO_OneEq & anIO)
{
    if (!mVEq.empty())
    {
         MMVII_INTERNAL_ASSERT_tiny
         (
             (anIO.mTmpUK.size()==mVEq.back().mTmpUK.size()),
	     "Variable size of temporaries"
         );
    }
    MMVII_INTERNAL_ASSERT_tiny(anIO.IsOk(),"Bad size for cInputOutputRSNL");

    mVEq.push_back(anIO);
    mNbEq += anIO.mVals.size();
    // A priori there is no use to less or equal equation, this doesnt give any constraint
    if (mNbEq > anIO.mTmpUK.size())
    {
        mOk = true; 
    }
}


template <class Type> 
    const std::vector<cInputOutputRSNL<Type> >& 
          cSetIORSNL_SameTmp<Type>::AllEq() const
{
     return mVEq;
}

template <class Type> void cSetIORSNL_SameTmp<Type>::AssertOk() const
{
      MMVII_INTERNAL_ASSERT_tiny(mOk,"Not enough eq to use tmp unknowns");
}

template <class Type> size_t cSetIORSNL_SameTmp<Type>::NbTmpUk() const
{
    return mVEq.at(0).mTmpUK.size();
}

/* ************************************************************ */
/*                                                              */
/*                cResidualWeighter                             */
/*                                                              */
/* ************************************************************ */

template <class Type>  cResidualWeighter<Type>::cResidualWeighter()
{
}

template <class Type>  std::vector<Type>  cResidualWeighter<Type>::WeightOfResidual(const tStdVect & aVResidual) const
{
	return tStdVect(aVResidual.size(),1.0);
}


/* ************************************************************ */
/*                                                              */
/*                cResolSysNonLinear                            */
/*                                                              */
/* ************************************************************ */

template <class Type> cResolSysNonLinear<Type>:: cResolSysNonLinear(tSysSR * aSys,const tDVect & aInitSol) :
    mNbVar      (aInitSol.Sz()),
    mCurGlobSol (aInitSol.Dup()),
    mSys        (aSys)
{
}

template <class Type> cResolSysNonLinear<Type>::cResolSysNonLinear(eModeSSR aMode,const tDVect & aInitSol) :
    cResolSysNonLinear<Type>  (cLinearOverCstrSys<Type>::AllocSSR(aMode,aInitSol.Sz()),aInitSol)
{
}

template <class Type> void   cResolSysNonLinear<Type>::AddEqFixVar(const int & aNumV,const Type & aVal,const Type& aWeight)
{
     tSVect aSV;
     aSV.AddIV(aNumV,1.0);
     mSys->AddObservation(aWeight,aSV,aVal);
}


template <class Type> const cDenseVect<Type> & cResolSysNonLinear<Type>::CurGlobSol() const 
{
    return mCurGlobSol;
}
template <class Type> const Type & cResolSysNonLinear<Type>::CurSol(int aNumV) const
{
    return mCurGlobSol(aNumV);
}

template <class Type> const cDenseVect<Type> & cResolSysNonLinear<Type>::SolveUpdateReset() 
{
    // mCurGlobSol += mSys->Solve();
    mCurGlobSol += mSys->SparseSolve();
    mSys->Reset();

    return mCurGlobSol;
}


template <class Type> void   cResolSysNonLinear<Type>::AddEqFixCurVar(const int & aNumV,const Type& aWeight)
{
     AddEqFixVar(aNumV,mCurGlobSol(aNumV),aWeight);
}

template <class Type> void cResolSysNonLinear<Type>::CalcAndAddObs
                           (
                                  tCalc * aCalcVal,
			          const tVectInd & aVInd,
				  const tStdVect& aVObs,
				  const tResidualW & aWeigther
                            )
{
    std::vector<tIO_TSNL> aVIO(1,tIO_TSNL(aVInd,aVObs));

    CalcVal(aCalcVal,aVIO,true,aWeigther);
    AddObs(aVIO);
}


template <class Type> cResolSysNonLinear<Type>::~cResolSysNonLinear()
{
    delete mSys;
}


template <class Type> void cResolSysNonLinear<Type>::AddObs ( const std::vector<tIO_TSNL>& aVIO)
{
      // Parse all the linearized equation
      for (const auto & aIO : aVIO)
      {
	  // check we dont use temporary value
          MMVII_INTERNAL_ASSERT_tiny(aIO.mTmpUK.empty(),"Cannot use tmp uk w/o Schurr complement");

	  // parse all values
	  for (size_t aKVal=0 ; aKVal<aIO.mVals.size() ; aKVal++)
	  {
	      Type aW = aIO.WeightOfKthResisual(aKVal);
	      if (aW>0)
	      {
	         tSVect aSV;
		 const tStdVect & aVDer = aIO.mDers[aKVal];
	         for (size_t aKUk=0 ; aKUk<aIO.mVInd.size() ; aKUk++)
                 {
                     aSV.AddIV(aIO.mVInd[aKUk],aVDer[aKUk]);
	         }
		 // Note the minus sign :  F(X0+dx) = F(X0) + Gx.dx   =>   Gx.dx = -F(X0)
	         mSys->AddObservation(aW,aSV,-aIO.mVals[aKVal]);
	      }
	  }

      }
}


template <class Type> void   cResolSysNonLinear<Type>::AddEq2Subst 
                             (
			          cSetIORSNL_SameTmp<Type> & aSetIO,tCalc * aCalc,const tVectInd & aVInd,const tStdVect& aVTmp,
			          const tStdVect& aVObs,const tResidualW & aWeighter
			     )
{
    std::vector<tIO_TSNL> aVIO(1,tIO_TSNL(aVInd,aVTmp,aVObs));
    CalcVal(aCalc,aVIO,true,aWeighter);

    aSetIO.AddOneEq(aVIO.at(0));
}
			     
template <class Type> void cResolSysNonLinear<Type>::AddObsWithTmpUK (const cSetIORSNL_SameTmp<Type> & aSetIO)
{
    mSys->AddObsWithTmpUK(aSetIO);
}

template <class Type> void   cResolSysNonLinear<Type>::CalcVal
                             (
			          tCalc * aCalcVal,
				  std::vector<tIO_TSNL>& aVIO,
				  bool WithDer,
				  const tResidualW & aWeighter
                              )
{
      MMVII_INTERNAL_ASSERT_tiny(aCalcVal->NbInBuf()==0,"Buff not empty");

      // Put input data
      for (const auto & aIO : aVIO)
      {
          tStdVect aVCoord;
	  // transferate global coordinates
	  for (const auto & anInd : aIO.mVInd)
              aVCoord.push_back(mCurGlobSol(anInd));
	  // transferate potential temporary coordinates
	  for (const  auto & aVal : aIO.mTmpUK)
              aVCoord.push_back(aVal);
	  //  Add equation in buffer
          aCalcVal->PushNewEvals(aVCoord,aIO.mObs);
      }
      // Make the computation
      aCalcVal->EvalAndClear();

      // Put output data
      size_t aNbEl = aCalcVal->NbElem();
      size_t aNbUk = aCalcVal->NbUk();
      // Parse all equation computed
      for (int aNumPush=0 ; aNumPush<int(aVIO.size()) ; aNumPush++)
      {
           auto & aIO = aVIO.at(aNumPush);
	   aIO.mVals = tStdVect(aNbEl);
	   if (WithDer)
	       aIO.mDers = std::vector(aNbEl,tStdVect( aNbUk));  // initialize vector to good size
	   // parse different values of each equation
           for (size_t aKEl=0; aKEl<aNbEl  ; aKEl++)
	   {
               aIO.mVals.at(aKEl) = aCalcVal->ValComp(aNumPush,aKEl);
	       if (WithDer)
	       {
	            // parse  all unknowns
	            for (size_t aKUk =0 ; aKUk<aNbUk ; aKUk++)
		    {
                        aIO.mDers.at(aKEl).at(aKUk) = aCalcVal->DerComp(aNumPush,aKEl,aKUk);
		    }
               }
	   }
           aIO.mWeights = aWeighter.WeightOfResidual(aIO.mVals);
      }
}
#if (0)

#endif


/* ************************************************************ */
/*                                                              */
/*                  INSTANTIATION                               */
/*                                                              */
/* ************************************************************ */

#define INSTANTIATE_RESOLSYSNL(TYPE)\
template class  cInputOutputRSNL<TYPE>;\
template class  cSetIORSNL_SameTmp<TYPE>;\
template class  cResidualWeighter<TYPE>;\
template class  cResolSysNonLinear<TYPE>;

INSTANTIATE_RESOLSYSNL(tREAL4)
INSTANTIATE_RESOLSYSNL(tREAL8)
INSTANTIATE_RESOLSYSNL(tREAL16)


};
