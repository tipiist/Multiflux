using GrpcPricing.Protos;
using HedgingEngine.Helpers;
using MarketData;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TimeHandler;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace HedgingEngine
{
    public class Portfolio
    {
        public Dictionary<string, double> Composition_ { get; set; }
        public double PortfolioValue_ { get; set; }
        public DateTime CurrentDate_ { get; set; }
        public double RiskFreeCash_ { get; set; }
        public DateTime LastRebDate_ { get; set; }

        public Portfolio() {
            Composition_ = new Dictionary<string, double>();
        }
        public Portfolio(Dictionary<string, double>  composition, double portfolioValue, DateTime tempCurrentDate, double riskFreeCash, DateTime lastRebDate)
        {
            Composition_ = composition;
            PortfolioValue_ = portfolioValue;
            CurrentDate_ = tempCurrentDate;
            RiskFreeCash_ = riskFreeCash;
            LastRebDate_ = lastRebDate;
        }


 public void RebalancePortfolio(DataFeed feed, PricingOutput output, double interestRate, int nbOfDaysPerYear, HelperMethods _helper) 
        {
            double riskyCash = _helper.ComputeSumProd(Composition_, feed.SpotList);
            MathDateConverter obj = new MathDateConverter(nbOfDaysPerYear);
            double capi = Math.Exp(interestRate * obj.ConvertToMathDistance(CurrentDate_, feed.Date));

            Dictionary<string, int> shareIds = new Dictionary<string, int>();
            _helper.GetIdsAndShareSpots(shareIds, feed.SpotList);

            PortfolioValue_ = RiskFreeCash_ * capi + riskyCash;
            _helper.SetComposition(this.Composition_, shareIds, output.Deltas.ToArray());
            LastRebDate_ = feed.Date;
            RiskFreeCash_ = PortfolioValue_ - _helper.ComputeSumProd(Composition_, feed.SpotList);
            CurrentDate_ = feed.Date;
        }
    }
}
