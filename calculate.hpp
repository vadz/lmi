// Run life insurance illustrations and composites.
//
// Copyright (C) 2001, 2002, 2004, 2005 Gregory W. Chicares.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// http://savannah.nongnu.org/projects/lmi
// email: <chicares@cox.net>
// snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

// $Id: calculate.hpp,v 1.5 2005-04-15 13:57:21 chicares Exp $

#ifndef calculate_hpp
#define calculate_hpp

#include "config.hpp"

#include "alert.hpp"
#include "inputillus.hpp"
#include "inputs.hpp"
#include "ledger.hpp"
#include "ledger_invariant.hpp"
#include "ledger_variant.hpp"
#include "ledgervalues.hpp"
#include "multiple_cell_document.hpp"
#include "single_cell_document.hpp"
#include "timer.hpp"

#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

struct RunIllustration
    :public std::unary_function<IllusInputParms, void>
{
    explicit RunIllustration(std::ostream& aOutputDest)
        :OutputDest           (aOutputDest)
        ,time_for_calculations(0.0)
        ,time_for_output      (0.0)
        {}

    void operator()(IllusInputParms a_input)
        {
        Timer timer;
        IllusVal IV;
        IV.Run(a_input);
        time_for_calculations += timer.Stop().Result();

        timer.Reset().Start();
        IV.Print(OutputDest);
        time_for_output       += timer.Stop().Result();
        }

    std::ostream& OutputDest;

    double time_for_calculations;
    double time_for_output;
};

struct RunIllustrationFromFile
    :public RunIllustration
{
    explicit RunIllustrationFromFile(std::ostream& aOutputDest)
        :RunIllustration(aOutputDest)
        ,time_for_input (0.0)
        {}
    void operator()(std::string a_filename)
        {
        Timer timer;
        std::ifstream is
            (a_filename.c_str()
            ,   std::ios_base::in
              | std::ios_base::binary
            );
/* TODO ?? expunge
        IllusInputParms input;
        is >> input;
*/
        if(!is)
            {
            throw std::runtime_error("Unable to open file " + a_filename);
            }
        single_cell_document doc;
        is >> doc;
        IllusInputParms input(doc.input_data());
        time_for_input += timer.Stop().Result();
        RunIllustration::operator()(input);
        }
    double time_for_input;
};

struct RunCensus
    :public std::unary_function<std::vector<IllusInputParms>, void>
{
    explicit RunCensus(std::ostream& aOutputDest)
        :OutputDest           (aOutputDest)
// TODO ?? Hardcoded ledger type must be changed.
        ,XXXComposite         (e_ledger_type(e_ill_reg), 100, true)
        ,time_for_calculations(0.0)
        ,time_for_output      (0.0)
        {}

    void operator()(std::vector<IllusInputParms> a_input)
        {
        if(a_input.empty())
            {
            return;
            }

        Timer timer;
        for
            (std::vector<IllusInputParms>::iterator lives_it = a_input.begin()
            ;lives_it != a_input.end()
            ;++lives_it
            )
            {
            std::auto_ptr<IllusVal> IV(new IllusVal());
            IV->Run(*lives_it);
// TODO ?? Pick one:            
//            Composite.operator+=(IV->ledger());
            XXXComposite.PlusEq(IV->ledger());
            }

        time_for_calculations += timer.Stop().Result();

        timer.Reset().Start();
        IllusVal Composite(&XXXComposite);
        Composite.Print(OutputDest);

        time_for_output       += timer.Stop().Result();
        }

    std::ostream& OutputDest;

    Ledger XXXComposite;

    double time_for_calculations;
    double time_for_output;
};

//============================================================================
// TODO ?? Remove this.
// TODO ?? expunge template<class Filename>
struct RunCensusDeprecated
// TODO ?? expunge    :public std::unary_function<Filename, void>
    :public std::unary_function<void, void>
{
    RunCensusDeprecated(){}
    void operator()(std::string const& a_filename)
        {
        Timer timer;
        std::ifstream is
            (a_filename.c_str()
            ,   std::ios_base::in
              | std::ios_base::binary
            );
        if(!is)
            {
            throw std::runtime_error("Unable to open file " + a_filename);
            }
#if 0
/* TODO ?? expunge
        IllusInputParms archetype;
        int n_lives;
        std::vector<IllusInputParms> lives;
        is >> archetype;
        if(!is)
            {
            throw std::runtime_error("Unable to read file " + a_filename);
            }

        is >> n_lives;
        // TODO ?? We don't like any limit, but anything larger is
        // likely to be a bug, so let's test this way for the time being.
        LMI_ASSERT(n_lives < 10000);
        lives.reserve(n_lives);

        for(int j = 0; j < n_lives; j++)
            {
            IllusInputParms life;
            is >> life;
            if(!is)
                {
                throw std::runtime_error("Unable to read file ");
                }
            lives.push_back(life);
            }
*/
#endif
        multiple_cell_document doc;
        is >> doc;
        if(!is)
            {
            throw std::runtime_error("Unable to read file " + a_filename);
            }

        std::cerr << "File:             " << a_filename << '\n';
        std::cerr << "    Input:        " << timer.Stop().Report() << '\n';

        // TODO ?? Why copy?
        std::vector<IllusInputParms> lives(doc.cell_parms());

        timer.Reset().Start();

// TODO ?? Use the stack instead of the heap.
        std::auto_ptr<IllusVal> Composite(new IllusVal(new Ledger(e_ledger_type(e_ill_reg), 100, true)));

        for
            (std::vector<IllusInputParms>::iterator lives_it = lives.begin()
            ;lives_it != lives.end()
            ;++lives_it
            )
            {
            std::auto_ptr<IllusVal> IV(new IllusVal());
            IV->Run(*lives_it);
            Composite->operator+=(IV->ledger());
            }

        std::cerr << "    Calculations: " << timer.Stop().Report() << '\n';

        timer.Reset().Start();
        Composite->Print(std::cout);
        std::cerr << "    Output:       " << timer.Stop().Report() << '\n';
        }
};

// COMPILER !! bc++5.5.1 doesn't like a template at the end of a file, so...
#if defined __BORLANDC__ && __BORLANDC__ >= 0x0550
extern int borland_workaround;
#endif

#endif // calculate_hpp

