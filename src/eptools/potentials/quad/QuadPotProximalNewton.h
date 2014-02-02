/* -------------------------------------------------------------------
 * LHOTSE: Toolbox for adaptive statistical models
 * -------------------------------------------------------------------
 * Project source file
 * Module: eptools
 * Desc.:  Header class QuadPotProximal
 * ------------------------------------------------------------------- */

/*
 * TODO:
 * - Mechanism to store the last recent proximal map solution and to use
 *   it in order to initialize the bracket
 */

#ifndef EPTOOLS_QUADPOTPROXIMAL_H
#define EPTOOLS_QUADPOTPROXIMAL_H

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "src/eptools/potentials/quad/QuadPotProximal.h"
#include "lhotse/optimize/FuncOneDim.h"

//BEGINNS(eptools)
  /**
   * Represents derivative f'(s) for proximal map criterion
   *   f(s) = rho l(s) + (1/2) (s - h)^2,    l(s) = -log t(s),
   *   f'(s) = rho l'(s) + s - h
   * Used to drive 'OneDimSolver'.
   *
   * @author  Matthias Seeger
   * @version %I% %G%
   */
  class QuadPotProximalNewton_Func1D : public FuncOneDim
  {
  protected:
    // Members

    const QuadraturePotential* quadPot; // Access to l'(s), l''(s)
    double h,rho;

  public:
    // Public methods

    QuadPotProximalNewton_Func1D(const QuadraturePotential* qpot,
				 double ph=0.0,double prho=1.0) :
      quadPot(qpot),h(ph),rho(prho) {
      if (!qpot->hasSecondDerivatives())
	throw InvalidParameterException(EXCEPT_MSG(""));
      setPars(ph,prho);
    }

    void setPars(double ph,double prho) {
      if (prho<(1e-16))
	throw InvalidParameterException(EXCEPT_MSG(""));
      h=ph; rho=prho;
    }

    bool hasDerivative() const {
      return true;
    }

    void eval(double x,double* f,double* df) {
      quadPot->eval(x,f,df);
      (*f) = rho*(*f)+x-h;
      (*df) = rho*(*df)+1.0;
    }
  };

  /**
   * Implements proximal map service 'proximal' by way of 1D Newton
   * ('OneDimSolver'). This is guaranteed to work for convex,
   * continuously differentiable functions, it may fail otherwise, even
   * if the function is unimodal.
   * <p>
   * An initial bracket has to be specified via 'initBracket'. Namely,
   * we search for a root of
   *   f'(s) = rho l'(s) + s - h
   * Since f''(s) = rho l''(s) + 1, this function is increasing if l(s)
   * is convex, so an initial bracket [L,R] must be s.t.
   *   f'(L) < 0,  f'(R) > 0
   * L must be supplied. If R is not supplied, it is determined
   * automatically. This may fail for non-convex l(s).
   *
   * @author  Matthias Seeger
   * @version %I% %G%
   */
  class QuadPotProximalNewton : public QuadPotProximal
  {
  protected:
    // Members

    Handle<QuadPotProximalNewton_Func1D> proxFun; // Function f'(s)
    double acc,facc;                              // See 'OneDimSolver'

  public:
    // Public methods

    /**
     * Constructor.
     *
     * @param pacc  See 'OneDimSolver::newton'. >0
     * @param pfacc "
     */
    QuadPotProximalNewton(double pacc,double pfacc);

    bool proximal(double h,double rho,double& sstar);

    /**
     * See header comment. The initial bracket is [L,R]. L has to be supplied,
     * R is optional ('r' is not used if <= 'l'). We require that
     *   f'(L) < 0,   f'(R) > 0.
     *
     * @param h   See 'proximal'
     * @param rho "
     * @param l   L ret. here
     * @param r   R ret. here (optional)
     */
    virtual void initBracket(double h,double rho,double& l,double& r) const = 0;
  };
//ENDNS

#endif