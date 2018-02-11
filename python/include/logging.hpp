//This file is part of Bertini 2.
//
//python/include/logging.hpp is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//python/include/logging.hpp is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with python/include/logging.hpp.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright(C) 2018 by Bertini2 Development Team
//
// See <http://www.gnu.org/licenses/> for a copy of the license, 
// as well as COPYING.  Bertini2 is provided with permitted 
// additional terms in the b2/licenses/ directory.

// individual authors of this file include:
//
//  Dani Brake
//  UWEC
//  Spring 2018
//
//
//  python/include/logging.hpp:  header file for exposing logging to python.

#pragma once

#include "python_common.hpp"
#include <bertini2/logging.hpp>

namespace bertini{ namespace python{


void ExportLogging();



}} // namespaces