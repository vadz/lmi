// GPT server test.
//
// Copyright (C) 1998, 2001, 2002, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Gregory W. Chicares.
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
// email: <gchicares@sbcglobal.net>
// snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

// $Id$

// The purpose of this file is to test the GPT server by loading it
// dynamically as a shared library.

// This is C++, but is largely written in C so that a C version may
// easily be written if desired.

#include LMI_PCH_HEADER
#ifdef __BORLANDC__
#   pragma hdrstop
#endif // __BORLANDC__

#include "ihs_server7702.hpp"

#include "handle_exceptions.hpp"
#include "path_utility.hpp"             // initialize_filesystem()
#include "so_attributes.hpp"

#if defined LMI_POSIX
#   include <dlfcn.h>                   // dlopen()
#elif defined LMI_MSW
#   include <windows.h>                 // LoadLibrary()
#else // Unknown platform.
#   error "Unknown platform. Consider contributing support."
#endif // Unknown platform.

#include <exception>                    // set_terminate()
#include <stdio.h>                      // fprintf()
#include <string.h>                     // strcmp()

#if !defined __cplusplus
typedef int     bool        ;
#endif // !defined __cplusplus
typedef char*   V_PolForm   ;
typedef int     E_UWBasis   ;
typedef int     E_Gender    ;
typedef int     E_Smoking   ;
typedef int     E_Class     ;
typedef int     E_State     ;
typedef int     E_7702DBOpt ;
typedef int     E_WPRating  ;
typedef int     E_ADDRating ;
typedef int     E_TblRating ;

struct input
{
    int         UniqueIdentifier;
    bool        IsIssuedToday;
    int         Duration;
    double      GrossNontaxableWithdrawal;
    double      Premium;
    double      DecreaseRequiredByContract;
    V_PolForm   ProductName;
    E_UWBasis   UnderwritingBasis;
    double      PremTaxLoadRate;
    double      TieredAssetChargeRate;
    double      LastFaceAmount;
    double      LeastFaceAmountEver;
    double      OldGuidelineLevelPremium;
    double      OldGuidelineSinglePremium;
    double      OldDeathBenefit;
    int         NewIssueAge;
    int         OldIssueAge;
    E_Gender    NewGender;
    E_Gender    OldGender;
    E_Smoking   NewSmoker;
    E_Smoking   OldSmoker;
    E_Class     NewUnderwritingClass;
    E_Class     OldUnderwritingClass;
    E_State     NewStateOfJurisdiction;
    E_State     OldStateOfJurisdiction;
    E_7702DBOpt NewDeathBenefitOption;
    E_7702DBOpt OldDeathBenefitOption;
    double      NewSpecifiedAmount;
    double      OldSpecifiedAmount;
    double      NewTermAmount;
    double      OldTermAmount;
    bool        NewWaiverOfPremiumInForce;
    bool        OldWaiverOfPremiumInForce;
    bool        NewPremiumsWaived;
    bool        OldPremiumsWaived;
    E_WPRating  NewWaiverOfPremiumRating;
    E_WPRating  OldWaiverOfPremiumRating;
    bool        NewAccidentalDeathInForce;
    bool        OldAccidentalDeathInForce;
    E_ADDRating NewAccidentalDeathRating;
    E_ADDRating OldAccidentalDeathRating;
    E_TblRating NewTableRating;
    E_TblRating OldTableRating;
    double      NewPermanentFlatAmount0;
    double      OldPermanentFlatAmount0;
    double      NewTemporaryFlatAmount0;
    double      OldTemporaryFlatAmount0;
    int         NewTemporaryFlatDuration0;
    int         OldTemporaryFlatDuration0;
    double      TargetPremium;
};

struct output
{
    int         UniqueIdentifier;
    int         Status;
    bool        AdjustableEventOccurred;
    double      GuidelineLevelPremium;
    double      GuidelineSinglePremium;
    double      GuidelineLevelPremiumPolicyA;
    double      GuidelineSinglePremiumPolicyA;
    double      GuidelineLevelPremiumPolicyB;
    double      GuidelineSinglePremiumPolicyB;
    double      GuidelineLevelPremiumPolicyC;
    double      GuidelineSinglePremiumPolicyC;
    double      Forceout;
    double      LeastFaceAmountEver;
    double      NewFaceAmount;
};

int main()
{
/*
    struct input s;

    s.UniqueIdentifier              = 12345;
    s.IsIssuedToday                 = 1;
    s.Duration                      = 0;
    s.GrossNontaxableWithdrawal     = 0.0;
    s.Premium                       = 10000;
    s.DecreaseRequiredByContract    = 0.0;
    s.ProductName                   = "sample";
    s.UnderwritingBasis             = "Medical";
    s.PremTaxLoadRate               = .02;
    s.TieredAssetChargeRate         = 0.0;
//    s.LastFaceAmount                = 1000000.0; // Apparently long obsolete.
    s.LeastFaceAmountEver           = 1000000.0;
    s.OldGuidelineLevelPremium      = 0.0;
    s.OldGuidelineSinglePremium     = 0.0;
//    s.OldDeathBenefit               = 1000000.0; // Apparently long obsolete.
    s.NewIssueAge                   = 45;
    s.OldIssueAge                   = 45;
    s.NewGender                     = "Male";
    s.OldGender                     = "Male";
    s.NewSmoker                     = "Nonsmoker";
    s.OldSmoker                     = "Nonsmoker";
    s.NewUnderwritingClass          = "Preferred";
    s.OldUnderwritingClass          = "Preferred";
    s.NewStateOfJurisdiction        = "CT";
    s.OldStateOfJurisdiction        = "CT";
    s.NewDeathBenefitOption         = "A";
    s.OldDeathBenefitOption         = "A";
    s.NewBenefitAmount              = 1000000.0;
    s.OldBenefitAmount              = 1000000.0;
    s.NewSpecifiedAmount            = 1000000.0;
    s.OldSpecifiedAmount            = 1000000.0;
    s.NewTermAmount                 = 0.0;
    s.OldTermAmount                 = 0.0;
    s.NewWaiverOfPremiumInForce     = 0;
    s.OldWaiverOfPremiumInForce     = 0;
    s.NewPremiumsWaived             = 0;
    s.OldPremiumsWaived             = 0;
    s.NewWaiverOfPremiumRating      = "None";
    s.OldWaiverOfPremiumRating      = "None";
    s.NewAccidentalDeathInForce     = 0;
    s.OldAccidentalDeathInForce     = 0;
    s.NewAccidentalDeathRating      = "None";
    s.OldAccidentalDeathRating      = "None";
    s.NewTableRating                = "None";
    s.OldTableRating                = "None";
    s.NewPermanentFlatAmount0       = 0.0;
    s.OldPermanentFlatAmount0       = 0.0;
    s.NewTemporaryFlatAmount0       = 0.0;
    s.OldTemporaryFlatAmount0       = 0.0;
    s.NewTemporaryFlatDuration0     = 0;
    s.OldTemporaryFlatDuration0     = 0;

    fprintf
        (stdout
        ,"%d %d %d %f %f %f %f %f %f %f %f %f %f %f\n"
        ,RunServer7702FromStruct(s)
        );
*/
    char c[] =
        "1 1 0 0 10000 0 " "sample" "\nMedical\n .02 0 1000000 0 0"
        " 45 45\nMale\nMale\nNonsmoker\nNonsmoker\nPreferred\nPreferred\nCT\nCT\nA\nA\n"
        " 1000000 1000000 1000000 1000000 0 0 0 0 0 0"
        "\nA=+25%\nA=+25%\n0 0\nNone\nNone\nP=+400%\nP=+400%\n"
        " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 55394.15"
        ;

    char e[] =
        "1 0 0 22110.68343118850680184551 239162.50350465354858897626 22110.6834311885068"
        "0184551 239162.50350465354858897626 0.00000000000000000000 0.0000000000000000000"
        "0 0.00000000000000000000 0.00000000000000000000\n"
        ;

    char o[16384];
/*
1000 iterations:
63 sec 20001230
    int j; for(j = 0; j < 1000; j++)
*/

    std::set_terminate(lmi_terminate_handler);
    try
        {
        // Absolute paths require "native" name-checking policy for msw.
        initialize_filesystem();
#if defined LMI_POSIX
        dlopen("gpt_server.so", RTLD_LAZY | RTLD_GLOBAL);
#elif defined LMI_MSW
        LoadLibrary("gpt_server.dll");
#else // Unknown platform.
#   error "Unknown platform. Consider contributing support."
#endif // Unknown platform.

        InitializeServer7702();
        RunServer7702FromString(c, o);
        if(0 != strcmp(o, e))
            {
            fprintf(stdout, "Error: %s\n", o);
            return 1;
            }
        }
    catch(...)
        {
        report_exception();
        return 2;
        }
}
