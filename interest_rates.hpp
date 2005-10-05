// Interest rates.
//
// Copyright (C) 2004, 2005 Gregory W. Chicares.
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

// $Id: interest_rates.hpp,v 1.3 2005-10-05 17:07:52 chicares Exp $

#ifndef interest_rates_hpp
#define interest_rates_hpp

#include "config.hpp"

#include "round_to.hpp"
#include "xenumtypes.hpp"

#include <vector>

// Calculate and store vectors of interest factors. Monthly rates must
// be calculated from annual input, and testing demonstrates that a
// naive implementation that does that on the fly makes the program
// noticeably slower, so efficiency is important here. Rates are
// calculated once and stored as annual or monthly vectors; that
// uses a lot of memory, but avoids performing exponential
// calculations repetetively, as for iterative solves. Net and gross
// rates, and annual and monthly rates, are stored where needed.

// Interest rates are generally stored as i (or, for different
// periodicity, i upper 12, e.g.) rather than as (1 + i). It might
// seem better, at first blush, to store (1 + i), but monthiversary
// calculations generally calculate an interest increment, round it,
// and then add it to the principal.

// UL potentially requires each of these distinct accounts:
//   general account
//   separate account
//   regular loan account
//   preferred loan account
// This implementation models only one separate account, using the
// weighted average of charges for all funds elected. Loan accounts
// may be commingled with the general account on the company's books,
// but they bear different interest rates and therefore must be
// distinguished here.

// Rates for different accounts vary across different axes.

// The general account and the loan accounts distinguish current from
// guaranteed rates; illustration-reg formats also require midpoint
// rates for these accounts. The separate account is needed only for
// NASD formats and generally doesn't have a guaranteed rate at all,
// but NASD requires distinguishing current from guaranteed separate-
// account charges, which affect net interest rates. Therefore, all
// accounts vary across the axis
//   {current, [midpoint,] guaranteed}
// where midpoint is used only for illustration-reg formats. For
// simplicity, the present implementation uses the same three-element
// axis for all formats, even though the midpoint basis is not used
// for NASD formats; perhaps someday it will be useful, e.g. if NAIC
// adopts an illustration reg for variable products or if some
// company wants to create a hybrid illustration that satisfies both
// sets of regulations.

// For the separate account, NASD requires a zero percent rate in
// addition to the input interest rate. Some companies may wish to
// show a third rate that is the average of those two: in this
// implementation, separate-account rates vary across the axis
//   {input, half-of-input, zero}
// for generality. This variation is not mapped to the
//   {current, [midpoint,] guaranteed}
// axis, because an investment return of zero is generally not
// guaranteed, and also because separate-account rates must vary
// across both these axes to reflect current versus guaranteed
// investment expenses, as described above.

// Loan accounts distinguish interest rates credited from those
// charged. Because interest is usually capitalized rather than paid
// in cash, the "charged" rate is called the "due" rate here. Unlike
// the other axes previously identified, this one doesn't depend on
// the basis upon which the illustration is run, so this distinction
// is recognized in this implementation not as an array dimension, but
// rather by distinguished variable names.

// The 'honeymoon' benefit and 7702A's deemed cash value have their
// own distinct interest rates, which vary across none of the above
// axes. Whether they are regarded as distinct 'accounts' or as
// artifacts embedded in other accounts is immaterial.

// An alternative not chosen here would be to treat interest rates
// as varying across a single axis with elements
//   curr charges, curr GA interest, SA interest = input if applicable
//   mdpt charges, mdpt GA interest (SA cannot be applicable)]
//   guar charges, guar GA interest, SA interest = input if applicable
//   curr charges, curr GA interest, SA interest = 0%
//   guar charges, guar GA interest, SA interest = 0%
//   curr charges, curr GA interest, SA interest = 1/2-input
//   guar charges, guar GA interest, SA interest = 1/2-input
// to accommodate the general and separate accounts, along with
//   regular   loan, credited
//   regular   loan, due
//   preferred loan, credited
//   preferred loan, due
//   DCV [and 7702 GPT interest rates]
//   honeymoon (two rates)
// That approach was rejected as ignoring the structure of the data.

// Rates are stored in arrays of vectors. Vectors of vectors would
// make copying trivial, but copying is unnecessary and forbidden.

// Member names use these components to keep names reasonably short:
//   Mly-     monthly
//   Ann-     annual
//   Guar-    guaranteed
//   Curr-    current
//   Prf-     preferred (loan)
//   Reg-     regular (loan--as opposed to preferred)
//   Cred-    credited (loan)
//   Due-     due (loan)
//   Ln-      loan
//   -Int     interest rate

class BasicValues;

class InterestRates
{
  public:
    InterestRates(BasicValues const&);
    ~InterestRates();

    std::vector<double> const& MlyGlpRate() const;
    std::vector<double> const& MlyGspRate() const;

    std::vector<double> const& GenAcctGrossRate
        (e_basis const& basis
        ) const;
    std::vector<double> const& GenAcctNetRate
        (e_basis       const& basis
        ,e_rate_period const& rate_period
        ) const;

    std::vector<double> const& SepAcctGrossRate
        (e_sep_acct_basis const& sepacct_basis
        ) const;
    std::vector<double> const& SepAcctNetRate
        (e_sep_acct_basis const& sepacct_basis
        ,e_basis const&          expense_basis
        ,e_rate_period const&    rate_period
        ) const;

    void DynamicMlySepAcctRate
        (e_basis const&          Basis
        ,e_sep_acct_basis const& SABasis
        ,int                     year
        ,double&                 MonthlySepAcctGrossRate
        ,double&                 AnnualSepAcctMandERate
        ,double&                 AnnualSepAcctIMFRate
        ,double&                 AnnualSepAcctMiscChargeRate
        ,double&                 AnnualSepAcctSVRate
        );

    std::vector<double> const& RegLnCredRate
        (e_basis       const& basis
        ,e_rate_period const& rate_period
        ) const;
    std::vector<double> const& RegLnDueRate
        (e_basis       const& basis
        ,e_rate_period const& rate_period
        ) const;
    std::vector<double> const& PrfLnCredRate
        (e_basis       const& basis
        ,e_rate_period const& rate_period
        ) const;
    std::vector<double> const& PrfLnDueRate
        (e_basis       const& basis
        ,e_rate_period const& rate_period
        ) const;

    std::vector<double> const& HoneymoonValueRate
        (e_basis       const& basis
        ,e_rate_period const& rate_period
        ) const;
    std::vector<double> const& PostHoneymoonGenAcctRate
        (e_basis       const& basis
        ,e_rate_period const& rate_period
        ) const;

    std::vector<double> const& InvestmentManagementFee() const;
    std::vector<double> const& MAndERate
        (e_basis const& Basis
        ) const;

  private:
    InterestRates();
    InterestRates(InterestRates const&);
    InterestRates& operator=(InterestRates const&);

    void Initialize(); // TODO ?? Implementation needs work.
    void Initialize(BasicValues const&);
    void InitializeGeneralAccountRates();
    void InitializeSeparateAccountRates();
    void InitializeLoanRates();
    void InitializeHoneymoonRates();
    void Initialize7702Rates();

    int                Length_;
    round_to<double>   RoundIntRate_;
    round_to<double>   Round7702Rate_;

    std::vector<double> Zero_;

    // General account interest rates.
//    bool NeedGenAcctRates_; // TODO ?? Would this be useful?
    bool NeedMidpointRates_;
    e_int_rate_type GenAcctRateType_;
    std::vector<double> GenAcctGrossRate_
        [n_illreg_bases]
        ;
    std::vector<double> GenAcctNetRate_
        [n_rate_periods]
        [n_illreg_bases]
        ;
    std::vector<double> GenAcctSpread_;

    // Separate account interest rates.
    bool NeedSepAcctRates_;
    e_int_rate_type SepAcctRateType_;
    std::vector<double> SepAcctGrossRate_
        [n_sepacct_bases]
        ;
    std::vector<double> SepAcctNetRate_
        [n_rate_periods]
        [n_illreg_bases]
        [n_sepacct_bases]
        ;
    e_spread_method SepAcctSpreadMethod_;
    std::vector<double> SepAcctFloor_;
    std::vector<double> Stabilizer_; // TODO ?? Obsolete?
    std::vector<double> AmortLoad_;
    std::vector<double> ExtraSepAcctCharge_;
    std::vector<double> InvestmentManagementFee_;
    std::vector<double> MAndERate_[n_illreg_bases];

    // Loan interest rates.
    bool NeedLoanRates_;
    e_loan_rate_type LoanRateType_;
    std::vector<double> PublishedLoanRate_;
    std::vector<double> RegLnCredRate_
        [n_rate_periods]
        [n_illreg_bases]
        ;
    std::vector<double> RegLnDueRate_
        [n_rate_periods]
        [n_illreg_bases]
        ;
    bool NeedPrefLoanRates_;
    std::vector<double> PrfLnCredRate_
        [n_rate_periods]
        [n_illreg_bases]
        ;
    std::vector<double> PrfLnDueRate_
        [n_rate_periods]
        [n_illreg_bases]
        ;
    std::vector<double> RegLoanSpread_[n_illreg_bases];
    std::vector<double> PrfLoanSpread_[n_illreg_bases];

    // Honeymoon interest rates.
    bool NeedHoneymoonRates_;
    std::vector<double> HoneymoonValueRate_
        [n_rate_periods]
        [n_illreg_bases]
        ;
    std::vector<double> PostHoneymoonGenAcctRate_
        [n_rate_periods]
        [n_illreg_bases]
        ;
    std::vector<double> HoneymoonValueSpread_;
    std::vector<double> PostHoneymoonSpread_;

    // GLP and GSP interest rates. DCV uses the GLP rate.
    std::vector<double> SpreadFor7702_;
    std::vector<double> MlyGlpRate_;
    std::vector<double> MlyGspRate_;
};

inline std::vector<double> const& InterestRates::GenAcctGrossRate
    (e_basis const& basis
    ) const
    {return GenAcctGrossRate_[basis.value()];}

inline std::vector<double> const& InterestRates::GenAcctNetRate
    (e_basis       const& basis
    ,e_rate_period const& rate_period
    ) const
    {
    return GenAcctNetRate_
        [rate_period.value()]
        [basis      .value()]
        ;
    }

inline std::vector<double> const& InterestRates::SepAcctGrossRate
    (e_sep_acct_basis const& sepacct_basis
    ) const
    {return SepAcctGrossRate_[sepacct_basis.value()];}

inline std::vector<double> const& InterestRates::SepAcctNetRate
    (e_sep_acct_basis const& sepacct_basis
    ,e_basis          const& expense_basis
    ,e_rate_period    const& rate_period
    ) const
    {
    return SepAcctNetRate_
        [rate_period  .value()]
        [expense_basis.value()]
        [sepacct_basis.value()]
        ;
    }

inline std::vector<double> const& InterestRates::InvestmentManagementFee() const
    {return InvestmentManagementFee_;}

inline std::vector<double> const& InterestRates::MAndERate
    (e_basis const& Basis
    ) const
    {return MAndERate_[Basis.value()];}

inline std::vector<double> const& InterestRates::RegLnCredRate
    (e_basis       const& basis
    ,e_rate_period const& rate_period
    ) const
    {
    return RegLnCredRate_
        [rate_period.value()]
        [basis      .value()]
        ;
    }

inline std::vector<double> const& InterestRates::RegLnDueRate
    (e_basis       const& basis
    ,e_rate_period const& rate_period
    ) const
    {
    return RegLnDueRate_
        [rate_period.value()]
        [basis      .value()]
        ;
    }

inline std::vector<double> const& InterestRates::PrfLnCredRate
    (e_basis       const& basis
    ,e_rate_period const& rate_period
    ) const
    {
    return PrfLnCredRate_
        [rate_period.value()]
        [basis      .value()]
        ;
    }

inline std::vector<double> const& InterestRates::PrfLnDueRate
    (e_basis       const& basis
    ,e_rate_period const& rate_period
    ) const
    {
    return RegLnCredRate_
        [rate_period.value()]
        [basis      .value()]
        ;
    }

inline std::vector<double> const& InterestRates::HoneymoonValueRate
    (e_basis       const& basis
    ,e_rate_period const& rate_period
    ) const
    {
    return HoneymoonValueRate_
        [rate_period.value()]
        [basis      .value()]
        ;
    }

inline std::vector<double> const& InterestRates::PostHoneymoonGenAcctRate
    (e_basis       const& basis
    ,e_rate_period const& rate_period
    ) const
    {
    return PostHoneymoonGenAcctRate_
        [rate_period.value()]
        [basis      .value()]
        ;
    }

inline std::vector<double> const& InterestRates::MlyGlpRate() const
    {return MlyGlpRate_;}
inline std::vector<double> const& InterestRates::MlyGspRate() const
    {return MlyGspRate_;}

#endif // interest_rates_hpp

