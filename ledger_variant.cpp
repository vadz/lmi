// Ledger data that vary by basis.
//
// Copyright (C) 1998, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 Gregory W. Chicares.
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
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
//
// http://savannah.nongnu.org/projects/lmi
// email: <chicares@cox.net>
// snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

// $Id: ledger_variant.cpp,v 1.21 2008-01-01 18:29:47 chicares Exp $

#ifdef __BORLANDC__
#   include "pchfile.hpp"
#   pragma hdrstop
#endif // __BORLANDC__

#include "ledger_variant.hpp"

#include "assert_lmi.hpp"
#include "basic_values.hpp"
#include "database.hpp" // Used only for initial loan rate.
#include "dbnames.hpp"  // Used only for initial loan rate.
#include "inputs.hpp"
#include "inputstatus.hpp"
#include "interest_rates.hpp"
#include "outlay.hpp"

#include <algorithm>
#include <ostream>

//============================================================================
LedgerVariant::LedgerVariant(int len)
    :LedgerBase(len)
    ,FullyInitialized(false)
{
    Alloc(len);
}

//============================================================================
LedgerVariant::LedgerVariant(LedgerVariant const& obj)
    :LedgerBase(obj)
    ,FullyInitialized(false)
{
    Alloc(obj.GetLength());
    Copy(obj);
}

//============================================================================
LedgerVariant& LedgerVariant::operator=(LedgerVariant const& obj)
{
    if(this != &obj)
        {
        LedgerBase::operator=(obj);
        Destroy();
        Alloc(obj.Length);
        Copy(obj);
        }
    return *this;
}

//============================================================================
LedgerVariant::~LedgerVariant()
{
    Destroy();
}

//============================================================================
void LedgerVariant::Alloc(int len)
{
    Length  = len;

    BegYearVectors  ["COICharge"            ] = &COICharge              ;
    BegYearVectors  ["ExpenseCharges"       ] = &ExpenseCharges         ;

    // Account value released on death might at first seem to be
    // an end-of-year item, since we use curtate mortality and the
    // release must occur at the end of the year. It is actually a
    // beginning-of-year item because it is q times account value,
    // and q is as of the beginning of the year. Perhaps more
    // importantly, it is intended for use on a financial-statement
    // basis, which states values for an *interval* of a year
    // rather than at either of the year's endpoints. The same is
    // true of various other items such as policy fee and interest
    // credited as used here.
    BegYearVectors  ["AVRelOnDeath"         ] = &AVRelOnDeath           ;
    BegYearVectors  ["NetIntCredited"       ] = &NetIntCredited         ;
    BegYearVectors  ["GrossIntCredited"     ] = &GrossIntCredited       ;
    BegYearVectors  ["LoanIntAccrued"       ] = &LoanIntAccrued         ;

    BegYearVectors  ["NetCOICharge"         ] = &NetCOICharge           ;

    ForborneVectors ["ExperienceReserve"    ] = &ExperienceReserve      ;

    BegYearVectors  ["PolicyFee"            ] = &PolicyFee              ;
    BegYearVectors  ["PremTaxLoad"          ] = &PremTaxLoad            ;
    BegYearVectors  ["DacTaxLoad"           ] = &DacTaxLoad             ;
    BegYearVectors  ["SpecAmtLoad"          ] = &SpecAmtLoad            ;
    BegYearVectors  ["SepAcctLoad"          ] = &SepAcctLoad            ;

    // Deaths are assumed to come at the end of the year only; but
    // they're discounted by the proportion in force at the beginning.
    BegYearVectors  ["ClaimsPaid"           ] = &ClaimsPaid             ;
    BegYearVectors  ["DeathProceedsPaid"    ] = &DeathProceedsPaid      ;
    BegYearVectors  ["NetClaims"            ] = &NetClaims              ;
    BegYearVectors  ["NetPmt"               ] = &NetPmt                 ;

    EndYearVectors  ["AcctVal"              ] = &AcctVal                ;
    EndYearVectors  ["DacTaxRsv"            ] = &DacTaxRsv              ;
    EndYearVectors  ["CSVNet"               ] = &CSVNet                 ;
    EndYearVectors  ["CV7702"               ] = &CV7702                 ;
    EndYearVectors  ["EOYDeathBft"          ] = &EOYDeathBft            ;
    EndYearVectors  ["PrefLoanBalance"      ] = &PrefLoanBalance        ;
    EndYearVectors  ["TotalLoanBalance"     ] = &TotalLoanBalance       ;
    EndYearVectors  ["ExcessLoan"           ] = &ExcessLoan             ;
    EndYearVectors  ["AvgDeathBft"          ] = &AvgDeathBft            ;
    EndYearVectors  ["SurrChg"              ] = &SurrChg                ;
    EndYearVectors  ["TermPurchased"        ] = &TermPurchased          ;
    EndYearVectors  ["BaseDeathBft"         ] = &BaseDeathBft           ;
    EndYearVectors  ["ProjectedCoiCharge"   ] = &ProjectedCoiCharge     ;

    OtherVectors    ["MlySAIntRate"         ] = &MlySAIntRate           ;
    OtherVectors    ["MlyGAIntRate"         ] = &MlyGAIntRate           ;
    OtherVectors    ["MlyHoneymoonValueRate"] = &MlyHoneymoonValueRate  ;
    OtherVectors    ["MlyPostHoneymoonRate" ] = &MlyPostHoneymoonRate   ;
    OtherVectors    ["AnnSAIntRate"         ] = &AnnSAIntRate           ;
    OtherVectors    ["AnnGAIntRate"         ] = &AnnGAIntRate           ;
    OtherVectors    ["AnnHoneymoonValueRate"] = &AnnHoneymoonValueRate  ;
    OtherVectors    ["AnnPostHoneymoonRate" ] = &AnnPostHoneymoonRate   ;
    OtherVectors    ["KFactor"              ] = &KFactor                ;

    OtherScalars    ["LapseMonth"           ] = &LapseMonth             ;
    OtherScalars    ["LapseYear"            ] = &LapseYear              ;
    OtherScalars    ["InitAnnLoanCredRate"  ] = &InitAnnLoanCredRate    ;
    OtherScalars    ["InitAnnGenAcctInt"    ] = &InitAnnGenAcctInt      ;
    OtherScalars    ["InitAnnSepAcctGrossInt"] = &InitAnnSepAcctGrossInt    ;
    OtherScalars    ["InitAnnSepAcctNetInt" ] = &InitAnnSepAcctNetInt   ;

    LedgerBase::Alloc();

    Init();
}

//============================================================================
void LedgerVariant::Copy(LedgerVariant const& obj)
{
    LedgerBase::Copy(obj);

    ExpAndGABasis           = obj.ExpAndGABasis     ;
    SABasis                 = obj.SABasis           ;
    FullyInitialized        = obj.FullyInitialized  ;
}

//============================================================================
void LedgerVariant::Destroy()
{
    FullyInitialized = false;
}

//============================================================================
void LedgerVariant::Init()
{
    // Initializes (almost) everything with zeros.
    LedgerBase::Initialize(GetLength());

    ExpAndGABasis           = e_basis(e_currbasis);
    SABasis                 = e_sep_acct_basis(e_sep_acct_full);

    LapseYear               = Length;
    LapseMonth              = 11;

    FullyInitialized        = false;
}

//============================================================================
void LedgerVariant::Init
    (BasicValues*     a_BV
    ,e_basis          a_ExpAndGABasis
    ,e_sep_acct_basis a_SABasis
    )
{
    Init(); // Zero out (almost) everything to start.

    ExpAndGABasis       = a_ExpAndGABasis;
    SABasis             = a_SABasis;

//  EOYDeathBft     =
//  AcctVal         =
//  CSVNet          =
//  CV7702          =
//  COICharge       =
//  ExpenseCharges  =
    MlySAIntRate               =
        a_BV->InterestRates_->SepAcctNetRate
            (a_SABasis
            ,a_ExpAndGABasis
            ,e_rate_period(e_monthly_rate)
            );
    MlyGAIntRate               =
        a_BV->InterestRates_->GenAcctNetRate
            (a_ExpAndGABasis
            ,e_rate_period(e_monthly_rate)
            );
    MlyHoneymoonValueRate      =
        a_BV->InterestRates_->HoneymoonValueRate
            (a_ExpAndGABasis
            ,e_rate_period(e_monthly_rate)
            );
    MlyPostHoneymoonRate       =
        a_BV->InterestRates_->PostHoneymoonGenAcctRate
            (a_ExpAndGABasis
            ,e_rate_period(e_monthly_rate)
            );
    AnnSAIntRate               =
        a_BV->InterestRates_->SepAcctNetRate
            (a_SABasis
            ,a_ExpAndGABasis
            ,e_rate_period(e_annual_rate)
            );
    AnnGAIntRate               =
        a_BV->InterestRates_->GenAcctNetRate
            (a_ExpAndGABasis
            ,e_rate_period(e_annual_rate)
            );
    AnnHoneymoonValueRate      =
        a_BV->InterestRates_->HoneymoonValueRate
            (a_ExpAndGABasis
            ,e_rate_period(e_annual_rate)
            );
    AnnPostHoneymoonRate       =
        a_BV->InterestRates_->PostHoneymoonGenAcctRate
            (a_ExpAndGABasis
            ,e_rate_period(e_annual_rate)
            );

//  PrefLoanBalance =
//  TotalLoanBalance=
//  ExcessLoan      =
//  AvgDeathBft     =
//  SurrChg         =
//  TermPurchased   =
//  BaseDeathBft    =
//  ProjectedCoiCharge =
//  KFactor         =

    InitAnnLoanCredRate = a_BV->InterestRates_->RegLnCredRate
        (a_ExpAndGABasis
        ,e_rate_period(e_annual_rate)
        )[0];

    InitAnnGenAcctInt = a_BV->InterestRates_->GenAcctNetRate
        (a_ExpAndGABasis
        ,e_rate_period(e_annual_rate)
        )
        [0]
        ;

    InitAnnSepAcctGrossInt = a_BV->InterestRates_->SepAcctGrossRate(a_SABasis)[0];

    InitAnnSepAcctNetInt = a_BV->InterestRates_->SepAcctNetRate
        (a_SABasis
        ,a_ExpAndGABasis
        ,e_rate_period(e_annual_rate)
        )
        [0]
        ;

    FullyInitialized = true;
}

//============================================================================
LedgerVariant& LedgerVariant::PlusEq
    (LedgerVariant const&      a_Addend
    ,std::vector<double> const& a_Inforce
    )
{
    LedgerBase::PlusEq(a_Addend, a_Inforce);

    // Make sure total (this) has enough years to add all years of a_Addend to.
    LMI_ASSERT(a_Addend.Length <= Length);

    // TODO ?? If interest rates vary across a census, this will be wrong.
    InitAnnGenAcctInt           = a_Addend.InitAnnGenAcctInt;
    InitAnnSepAcctGrossInt      = a_Addend.InitAnnSepAcctGrossInt;
    InitAnnSepAcctNetInt        = a_Addend.InitAnnSepAcctNetInt;
    InitAnnLoanCredRate         = a_Addend.InitAnnLoanCredRate;
    // ET !! This is of the form 'x = (lengthof x) take y'.
    for(int j = 0; j < a_Addend.Length; j++)
        {
        MlySAIntRate              [j]  = a_Addend.MlySAIntRate              [j];
        MlyGAIntRate              [j]  = a_Addend.MlyGAIntRate              [j];
        MlyHoneymoonValueRate     [j]  = a_Addend.MlyHoneymoonValueRate     [j];
        MlyPostHoneymoonRate      [j]  = a_Addend.MlyPostHoneymoonRate      [j];
        AnnSAIntRate              [j]  = a_Addend.AnnSAIntRate              [j];
        AnnGAIntRate              [j]  = a_Addend.AnnGAIntRate              [j];
        AnnHoneymoonValueRate     [j]  = a_Addend.AnnHoneymoonValueRate     [j];
        AnnPostHoneymoonRate      [j]  = a_Addend.AnnPostHoneymoonRate      [j];
        KFactor                   [j]  = a_Addend.KFactor                   [j];
        }

    LMI_ASSERT(a_Addend.ExpAndGABasis == ExpAndGABasis);
    LMI_ASSERT(a_Addend.SABasis       == SABasis);

    if(LapseYear == a_Addend.LapseYear)
        {
        LapseMonth              = std::max(LapseMonth, a_Addend.LapseMonth);
        }
    else if(LapseYear < a_Addend.LapseYear)
        {
        LapseMonth              = a_Addend.LapseMonth;
        }
    LapseYear                   = std::max(LapseYear, a_Addend.LapseYear);

    return *this;
}

//============================================================================
void LedgerVariant::RecordDynamicSepAcctRate
    (double annual_rate
    ,double monthly_rate
    ,int year
    )
{
    AnnSAIntRate[year] = annual_rate;
    if(0 == year)
        {
        InitAnnSepAcctNetInt = annual_rate;
        }
    MlySAIntRate[year] = monthly_rate;
}

//============================================================================
void LedgerVariant::UpdateCRC(CRC& a_crc) const
{
    LedgerBase::UpdateCRC(a_crc);
}

//============================================================================
void LedgerVariant::Spew(std::ostream& os) const
{
    os
        << "ExpAndGABasis"
        << "=="
        << ExpAndGABasis
        << '\n'
        ;
    os
        << "SABasis"
        << "=="
        << SABasis
        << '\n'
        ;

    LedgerBase::Spew(os);
}

ledger_map_holder::ledger_map_holder()
{
}

ledger_map_holder::ledger_map_holder(ledger_map const& z)
    :held_(z)
{
}

ledger_map_holder::ledger_map_holder(ledger_map_holder const& z)
    :held_(z.held())
{
}

ledger_map_holder& ledger_map_holder::operator=(ledger_map_holder const& z)
{
    held_ = z.held();
    return *this;
}

ledger_map_holder::~ledger_map_holder()
{
}

ledger_map const& ledger_map_holder::held() const
{
    return held_;
}

