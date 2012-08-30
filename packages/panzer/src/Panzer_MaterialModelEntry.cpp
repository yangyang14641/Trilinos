// @HEADER
// ***********************************************************************
//
//           Panzer: A partial differential equation assembly
//       engine for strongly coupled complex multiphysics systems
//                 Copyright (2011) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Roger P. Pawlowski (rppawlo@sandia.gov) and
// Eric C. Cyr (eccyr@sandia.gov)
// ***********************************************************************
// @HEADER

#include "Panzer_MaterialModelEntry.hpp"

//=======================================================================
//=======================================================================
panzer::MaterialModelEntry::MaterialModelEntry()
{ }

//=======================================================================
//=======================================================================
panzer::MaterialModelEntry::
MaterialModelEntry(const std::string factory_name) :
  m_factory_name(factory_name)
{ }

//=======================================================================
//=======================================================================
panzer::MaterialModelEntry::
MaterialModelEntry(const std::string factory_name,
		   const Teuchos::ParameterList& p) :
  m_factory_name(factory_name),
  m_params(p)
{ }

//=======================================================================
//=======================================================================
std::string panzer::MaterialModelEntry::factoryName() const
{
  return m_factory_name;
}

//=======================================================================
//=======================================================================
const Teuchos::ParameterList& panzer::MaterialModelEntry::params() const
{
  return m_params;
}

//=======================================================================
//=======================================================================
void panzer::MaterialModelEntry::operator=(const MaterialModelEntry& e)
{
  m_factory_name = e.m_factory_name;
  m_params = e.m_params;
}

//=======================================================================
//=======================================================================
bool panzer::MaterialModelEntry::operator==(const MaterialModelEntry& e) const
{
  return ( (m_factory_name == e.m_factory_name) &&
	   (this->m_params == e.m_params) );
}

//=======================================================================
//=======================================================================
bool panzer::MaterialModelEntry::operator!=(const MaterialModelEntry& e) const
{
  return (!(this->operator==(e)));
}

//=======================================================================
//=======================================================================
void panzer::MaterialModelEntry::print(std::ostream& os) const
{
  os << "Material Model Entry: " << m_factory_name;
}

//=======================================================================
//=======================================================================
std::ostream& 
panzer::operator<<(std::ostream& os, panzer::MaterialModelEntry& m)
{
  m.print(os);
  return os;
}