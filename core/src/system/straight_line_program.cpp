//This file is part of Bertini 2.
//
//straight_line_program.cpp is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//straight_line_program.cpp is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with straight_line_program.cpp.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright(C) 2021 by Bertini2 Development Team
//
// See <http://www.gnu.org/licenses/> for a copy of the license, 
// as well as COPYING.  Bertini2 is provided with permitted 
// additional terms in the b2/licenses/ directory.

// individual authors of this file include:
// silviana amethyst, university of wisconsin eau claire

#include "bertini2/system/straight_line_program.hpp"
#include "bertini2/system/system.hpp"



// SLP Stuff
namespace bertini{
	

	// the constructor
	StraightLineProgram::StraightLineProgram(System const& sys){
		this->num_total_functions_ = sys.NumTotalFunctions();

		std::cout << sys.NumTotalFunctions() << std::endl;

		SLPCompiler compiler;
		*this = compiler.Compile(sys);
	}

	void StraightLineProgram::precision(unsigned new_precision) const{

	}




	void StraightLineProgram::AddInstruction(Operation binary_op, size_t in_loc1, size_t in_loc2, size_t out_loc){
		this->instructions_.push_back(binary_op);
		this->instructions_.push_back(in_loc1);
		this->instructions_.push_back(in_loc2);
		this->instructions_.push_back(out_loc);
	}



    void StraightLineProgram::AddInstruction(Operation unary_op, size_t in_loc, size_t out_loc){
    	this->instructions_.push_back(binary_op);
		this->instructions_.push_back(in_loc);
		this->instructions_.push_back(out_loc);
    }





}







// stuff for SLPCompiler
namespace bertini{
	using SLP = StraightLineProgram;


    void SLPCompiler::Visit(node::Variable const& n){
		std::cout << "unimplemented visit to node of type Variable" << std::endl;
	}

	void SLPCompiler::Visit(node::Integer const& n){
		std::cout << "unimplemented visit to node of type Integer" << std::endl;
	}

	void SLPCompiler::Visit(node::Float const& n){
		std::cout << "unimplemented visit to node of type Float" << std::endl;
	}

	void SLPCompiler::Visit(node::Rational const& n){
		std::cout << "unimplemented visit to node of type Rational" << std::endl;
	}











	void SLPCompiler::Visit(node::Function const & f){
		std::cout << "unimplemented visit to node of type Function: " << f << std::endl;

		// put the location of the accepted node into memory, and copy into an output location.

		auto& n = f.entry_node();
		size_t location_entry;

		if (*n not in this->locations_encountered_symbols_)
			location_entry = n->Accept(*this); // think of calling Compile(n)
		else
			location_entry = this->locations_encountered_symbols_[*n];

		size_t location_this_node = next_available_mem_++;  //this->slp_under_construction.AddToMemory(f);

		slp_under_construction.AddInstruction(Assign, location_entry, location_this_node);
	}


	// arithmetic
	void SLPCompiler::Visit(node::SumOperator const & op){
    	std::cout << "visiting SumOperator: " << std::endl;

    	// this loop
    	// gets the locations of all the things we're going to add up.
    	std::vector<size_t> operand_locations;
    	for (auto& n : op.Operands()){

	    	if (*n not in this->locations_encountered_symbols_)
				operand_locations.push_back(n->Accept(*this)); // think of calling Compile(n)
			else
				operand_locations.push_back(this->locations_encountered_symbols_[*n]);
		}

		const auto& signs = op->GetSigns();

		// seed the loop by adding together the first two things, which must exist by hypothesis
		assert(op.Operands().size() >=2);

		size_t prev_result_loc = next_available_mem_;

		if (signs[0])
			slp_under_construction.AddInstruction(Add,operand_locations[0],operand_locations[1], next_available_mem_++);
		else
			slp_under_construction.AddInstruction(Subtract,operand_locations[0],operand_locations[1], next_available_mem_++);

		// this loop 
		// actually does the additions for the rest of the operands
		
		for (size_t ii{2}; ii<op.Operands().size()-1; ++ii){
			if (signs[ii-1])
				slp_under_construction.AddInstruction(Add,operand_locations[ii],prev_result_loc,next_available_mem_++)
			else
				slp_under_construction.AddInstruction(Subtract,operand_locations[ii],prev_result_loc,next_available_mem_++)
		}

		size_t location_this_node = next_available_mem_++; // this->slp_under_construction.AddToMemory(op);

    	// for each pair, look up the node in the memory structure in SLP



    		// add a SUM op to the instructions.
    		
    		// add/subtract the nodes together.  

    		// naive method: just loop over all operands, add/sub them up.

    		// improved option?: we could do this in a way to minimize numerical error.  the obvious loop is not good for accumulation of error.  instead, use Pairwise summation
    		// do pairs (0,1), (2,3), etc, 
    		// *then* add those temp vals together, until get to end.  
    		// see https://en.wikipedia.org/wiki/Pairwise_summation    	
    	

    	// loop over pairs of operands, not one by one as is currently done.
    }









	void SLPCompiler::Visit(node::Jacobian const& n){
		std::cout << "unimplemented visit to node of type Jacobian" << std::endl;

		this->slp_under_construction.AddToMemory(n);
	}

	void SLPCompiler::Visit(node::Differential const& n){
		std::cout << "unimplemented visit to node of type Differential" << std::endl;
	}



	void SLPCompiler::Visit(node::MultOperator const & op){
    	std::cout << "visiting MultOperator: " << std::endl;
    	for (auto& n : op.Operands()){
    		n->Accept(*this);
    		// multiply/divide the nodes together.  naive method is fine.  
    		
    	}
    }

	void SLPCompiler::Visit(node::IntegerPowerOperator const& n){
		std::cout << "unimplemented visit to node of type IntegerPowerOperator" << std::endl;
	}

	void SLPCompiler::Visit(node::PowerOperator const& n){
		std::cout << "unimplemented visit to node of type PowerOperator" << std::endl;
	}

	void SLPCompiler::Visit(node::ExpOperator const& n){
		std::cout << "unimplemented visit to node of type ExpOperator" << std::endl;
	}

	void SLPCompiler::Visit(node::LogOperator const& n){
		std::cout << "unimplemented visit to node of type LogOperator" << std::endl;
	}


	// the trig operators
	void SLPCompiler::Visit(node::SinOperator const& n){
		std::cout << "unimplemented visit to node of type SinOperator" << std::endl;
	}

	void SLPCompiler::Visit(node::ArcSinOperator const& n){
		std::cout << "unimplemented visit to node of type ArcSinOperator" << std::endl;
	}

	void SLPCompiler::Visit(node::CosOperator const& n){
		std::cout << "unimplemented visit to node of type CosOperator" << std::endl;
	}

	void SLPCompiler::Visit(node::ArcCosOperator const& n){
		std::cout << "unimplemented visit to node of type ArcCosOperator" << std::endl;
	}

	void SLPCompiler::Visit(node::TanOperator const& n){
		std::cout << "unimplemented visit to node of type TanOperator" << std::endl;
	}

	void SLPCompiler::Visit(node::ArcTanOperator const& n){
		std::cout << "unimplemented visit to node of type ArcTanOperator" << std::endl;
	}



    SLP SLPCompiler::Compile(System const& sys){
    	this->Clear();

    	std::cout << "Compiling system" << std::endl;
    	std::cout << "visiting functions" << std::endl;

    	for (int ii = 0; ii < sys.NumTotalFunctions(); ++ii)
    	{
    		auto f = sys.Function(ii);

    		std::cout << *(f) << std::endl;
    		f->Accept(*this);

    		locations_encountered_symbols_[f] = next_available_mem_++; // this is obviously wrong

    		std::cout << "post visit function" << std::endl;
    		/* code */
    	}

    	return SLP();
    }

    void SLPCompiler::Clear(){
        next_available_mem_ = 0; 
        locations_encountered_symbols_.clear();
    }
}
