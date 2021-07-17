//This file is part of Bertini 2.
//
//straight_line_program.hpp is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//straight_line_program.hpp is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with straight_line_program.hpp.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright(C) 2021 by Bertini2 Development Team
//
// See <http://www.gnu.org/licenses/> for a copy of the license, 
// as well as COPYING.  Bertini2 is provided with permitted 
// additional terms in the b2/licenses/ directory.

// individual authors of this file include:
// silviana amethyst, university of wisconsin eau claire

/**
\file straight_line_program.hpp 

\brief Provides the bertini::StraightLineProgram class.
*/

#ifndef BERTINI_SLP_HPP
#define BERTINI_SLP_HPP

#include <assert.h>
#include <vector>


#include "bertini2/mpfr_complex.hpp"
#include "bertini2/mpfr_extensions.hpp"
#include "bertini2/eigen_extensions.hpp"
#include "bertini2/function_tree/forward_declares.hpp"
#include "bertini2/detail/visitor.hpp"

// code copied from Bertini1's file include/bertini.h


/*

typedef struct
{
  int num_funcs;
  int num_hom_var_gp;
  int num_var_gp;
  int *type; // 0 - hom_var_gp, 1 - var_gp
  int *size; // size of the group of the user listed variables (total size = size + type)
} preproc_data;


typedef struct
{
  point_d funcVals;
  point_d parVals;
  vec_d parDer;
  mat_d Jv;
  mat_d Jp;
} eval_struct_d;

typedef struct
{
  point_mp funcVals;
  point_mp parVals;
  vec_mp parDer;
  mat_mp Jv;
  mat_mp Jp;
} eval_struct_mp;


The straight-line program structure.  This is the way that polynomials are stored internally.
typedef struct {
  int *prog;     //  The program instructions. (a big integer array)  
  int  size;     //  size of the instruction program.      
  int  memSize;  // Amount of memory it needs in workspace (for temp and final results).   
  num_t *nums;   // The array of real numbers. 
  int precision; // The precision at which evaluation should occur 
  
  // INFO NEEDED FOR M-HOM: 
  int num_var_gps;  // The total number of variable groups (i.e., m from m-hom).
  int *var_gp_sizes;  // The size of each of the groups. 
  int index_of_first_number_for_proj_trans;  // The address of the first number used in the projective transformation polynomials. 
                                                                                    
  // STOP LOCATIONS: 
  int  numInstAtEndUpdate; // instruction number at end of update. i.e. i = 0; while (i < numInstAtEndUpdate) .. 
  int  numInstAtEndParams; // instruction number at end of params. i.e. i = numInstAtEndUpdate; while (i < numInstAtEndParams) .. 
  int  numInstAtEndFnEval; // instruction number at end of function eval. i.e. i = numInstAtEndParams; while (i < numInstAtEndFnEval) .. 
  int  numInstAtEndPDeriv; // instruction number at end of param diff. i.e. i = numInstAtEndFnEval; while (i < numInstAtEndPDeriv) .. 
  int  numInstAtEndJvEval; // instruction number at end of Jv eval. i.e. i = numInstAtEndPDeriv; while (i < numInstAtEndJvEval) .. 
                           // for Jp eval: i = numInstAtEndJvEval; while (i < size) ..  

  // INPUT AMOUNTS: 
  int  numVars;  //  Number of variables in the function being computed.   
  int  numPathVars;  //  Number of path variables.  Ought to be 1 usually.   
  int  numNums;  //  Number of real numbers used in evaluation.  
  int  numConsts;  //  Number of constants.     
                                                                                   
  // OUTPUT AMOUNTS: 
  int  numPars;  //  Number of parameters   
  int  numFuncs; //  Number of coordinate functions in the homotopy.   
  int  numSubfuncs;  //  Number of subfunctions.  
                                                                                      
  // INPUT LOCATIONS: 
  int  inpVars;  //  Where the input variable values are stored.   
  int  inpPathVars;  //  Where the values of the path variables are stored.   
  int  IAddr;  //  Where the constant I is stored.  
  int  numAddr;  //  Where the first num_t is stored.  
  int  constAddr;  //  Where the first constant is stored.  
                                                                                      
  // OUTPUT LOCATIONS: 
  int  evalPars;  //  Where U(t), for given t, is stored.   
  int  evalDPars;  //  Where the derivatives of the parameters are stored.   
  int  evalFuncs;  //  Where H(x,t) is stored.   
  int  evalJVars;  //  Where the Jacobian w.r.t. vars is stored.   
  int  evalJPars;  //  Where the Jacobian w.r.t. pars is stored.   
  int  evalSubs;  //  Where the subfunctions are stored 
  int  evalJSubsV;  //  Where the derivatives of the subfunctions w.r.t. vars are stored.  
  int  evalJSubsP;  //  Where the derivatives of the subfunctions w.r.t. pars are stored.  
} prog_t;
*/



namespace bertini {

  class System; // a forward declaration, solving the circular inclusion problem


  enum Operation { // we'll start with the binary ones
    Add=      1 << 0,
    Subtract= 1 << 1,
    Multiply= 1 << 2,
    Divide=   1 << 3,
    Power=    1 << 4,
    Exp=      1 << 5,
    Log=      1 << 6,
    Negate=   1 << 7,
    Sin=      1 << 8,
    Cos=      1 << 9,
    Tan=      1 << 10,
    Asin=     1 << 11,
    Acos=     1 << 12,
    Atan=     1 << 13,
    Assign=   1 << 14,
  };

  const int BinaryOperations = Add|Subtract | Multiply|Divide | Power;
  const int TrigOperations   = Sin|Cos|Tan | Asin|Acos|Atan;
  const int UnaryOperations  = Exp|Log | Negate | Assign | TrigOperations;

  constexpr bool IsUnary(Operation op)
  {
    return op & UnaryOperations;
  }

  constexpr bool IsBinary(Operation op)
  {
    return op & BinaryOperations;
  }


	class StraightLineProgram{
    friend SLPCompiler;
    
  private:
    using Nd = std::shared_ptr<node::Node>;

	public:
    struct OutputLocations{
      size_t Functions{0};
      size_t Variables{0};
      size_t Jacobian{0};
      size_t TimeDeriv{0};
    };

    struct NumberOf{
      size_t Functions{0};
      size_t Variables{0};
      size_t Jacobian{0};
      size_t TimeDeriv{0};
    };

    /**
    The constructor -- how to make a SLP from a System.
    */
    StraightLineProgram(System const & sys);

    StraightLineProgram() = default;

		template<typename Derived>
		void Eval(Eigen::MatrixBase<Derived> const& variable_values) const
		{
      // 1. copy variable values into memory locations they're supposed to go in
      CopyVariableValues(variable_values);
		}


    template<typename Derived, typename ComplexT>
    void Eval(Eigen::MatrixBase<Derived> const& variable_values, ComplexT const& time) const
    {
      // 1. copy variable values into memory locations they're supposed to go in
      CopyVariableValues(variable_values);
      CopyPathVariable(time);
    }



    template<typename NumT>
    void GetFuncVals(Vec<NumT> & result) const{
      // 1. make container, size correctly.
      // 2. copy content
      // 3. return
    }

    template<typename NumT>
    void GetJacobian(Mat<NumT> & result) const{
      // 1. make container, size correctly.
      // 2. copy content
      // 3. return
    }


    template<typename NumT>
    void GetTimeDeriv(Vec<NumT> & result) const{
      // 1. make container, size correctly.
      // 2. copy content
      // 3. return
    }


    template<typename NumT>
    Vec<NumT> GetFuncVals() const{
      Vec<NumT> return_me;
      GetFuncVals(return_me);
      return return_me;
    }

    template<typename NumT>
    Mat<NumT> GetJacobian() const{
      Mat<NumT> return_me;
      GetJacobian(return_me);
      return return_me;
    }

    template<typename NumT>
    Vec<NumT> GetTimeDeriv() const{
      Vec<NumT> return_me;
      GetTimeDeriv(return_me);
      return return_me;
    }


    inline unsigned NumTotalFunctions() const{ return num_total_functions_;}

    inline unsigned NumVariables() const{ return num_variables_;}


		/**
		\brief Get the current precision of the SLP.
		*/
		inline
		unsigned precision() const
		{
			return precision_;
		}

    /*
    change the precision of the SLP.  Downsamples from the true values.
    */
		void precision(unsigned new_precision) const;


    bool HavePathVariable() const {
      throw std::runtime_error("calling unimplemented function HavePathVariable");
      return false;
    }
	private:

    template<typename Derived>
    void CopyVariableValues(Eigen::MatrixBase<Derived> const& variable_values) const{
      throw std::runtime_error("calling unimplemented function CopyVariableValues");
    }

    template<typename ComplexT>
    void CopyPathVariable(ComplexT const& time) const{
      if (!this->HavePathVariable())
        throw std::runtime_error("calling Eval with path variable, but system doesn't have one.");
      // then actually copy the path variable into where it goes in memory
    }


    void AddInstruction(Operation binary_op, size_t in_loc1, size_t in_loc2, size_t out_loc);
    void AddInstruction(Operation unary_op, size_t in_loc, size_t out_loc);

    unsigned precision_ = 0;
    unsigned num_total_functions_ = 0;
    unsigned num_variables_ = 0;

    NumberOf number_of_;
    OutputLocations output_locations_;
    OutputLocations input_locations_;

    std::vector<size_t> instructions_;
    std::tuple< std::vector<dbl_complex>, std::vector<mpfr_complex> > memory_;

    std::vector< mpfr_complex > true_values_;
	};
	

  class SLPCompiler : public VisitorBase, 
      // symbols and roots
      public Visitor<node::Variable>,
      public Visitor<node::Integer>,
      public Visitor<node::Float>,
      public Visitor<node::Rational>,
      public Visitor<node::Function>,
      public Visitor<node::Jacobian>,
      public Visitor<node::Differential>,

      // arithmetic
      public Visitor<node::SumOperator>,
      public Visitor<node::MultOperator>,
      public Visitor<node::IntegerPowerOperator>,
      public Visitor<node::PowerOperator>,
      public Visitor<node::ExpOperator>,
      public Visitor<node::LogOperator>,

      // the trig operators
      public Visitor<node::SinOperator>,
      public Visitor<node::ArcSinOperator>,
      public Visitor<node::CosOperator>,
      public Visitor<node::ArcCosOperator>,
      public Visitor<node::TanOperator>,
      public Visitor<node::ArcTanOperator>

      // these abstract base types left out, 
      // but commented here to explain why
      //    public Visitor<node::Operator>,// abstract
      //    public Visitor<node::UnaryOperator>,// abstract
      //    public Visitor<node::NaryOperator>,// abstract
      //    public Visitor<node::TrigOperator>,// abstract
  {
  private:
    using Nd = std::shared_ptr<node::Node>;
    using SLP = StraightLineProgram;

    public:

      SLP Compile(System const& sys);

      // symbols and roots
      virtual void Visit(node::Variable const& n);
      virtual void Visit(node::Integer const& n);
      virtual void Visit(node::Float const& n);
      virtual void Visit(node::Rational const& n);
      virtual void Visit(node::Function const& n);
      virtual void Visit(node::Jacobian const& n);
      virtual void Visit(node::Differential const& n);

      // arithmetic
      virtual void Visit(node::SumOperator const& n);
      virtual void Visit(node::MultOperator const& n);
      virtual void Visit(node::IntegerPowerOperator const& n);
      virtual void Visit(node::PowerOperator const& n);
      virtual void Visit(node::ExpOperator const& n);
      virtual void Visit(node::LogOperator const& n);

      // the trig operators
      virtual void Visit(node::SinOperator const& n);
      virtual void Visit(node::ArcSinOperator const& n);
      virtual void Visit(node::CosOperator const& n);
      virtual void Visit(node::ArcCosOperator const& n);
      virtual void Visit(node::TanOperator const& n);
      virtual void Visit(node::ArcTanOperator const& n);


    private:
      void Clear();

      size_t next_available_mem_ = 0;
      std::map<Nd, size_t> locations_encountered_symbols_;
  };



} // namespace bertini









#endif // for the ifndef include guards


