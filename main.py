import time

from polygon import websocket
from polygon.websocket.models import WebSocketMessage
from typing import List

c = websocket.WebSocketClient(
        api_key='*APIKEY*',
        feed='delayed.polygon.io',
        market='stocks',
        subscriptions=["AM.MMM,AM.AOS,AM.ABT,AM.ABBV,AM.ABMD,AM.ACN,AM.ATVI,AM.ADM,AM.ADBE,AM.AAP,AM.AMD,AM.AES,AM.AFL,AM.A,AM.APD,AM.AKAM,AM.ALB,AM.ALK,AM.ARE,AM.ALGN,AM.ALLE,AM.LNT,AM.ALL,AM.GOOGL,AM.GOOG,AM.MO,AM.AMZN,AM.AMCR,AM.AEE,AM.AAL,AM.AEP,AM.AXP,AM.AIG,AM.AMT,AM.AWK,AM.AMP,AM.ABC,AM.AME,AM.AMGN,AM.APH,AM.ADI,AM.ANSS,AM.ANTM,AM.AON,AM.APA,AM.AAPL,AM.AMAT,AM.APTV,AM.ANET,AM.AJG,AM.AIZ,AM.T,AM.ATO,AM.ADSK,AM.ADP,AM.AZO,AM.AVB,AM.AVY,AM.BKR,AM.BLL,AM.BAC,AM.BBWI,AM.BAX,AM.BDX,AM.BRK.B,AM.BBY,AM.BIO,AM.TECH,AM.BIIB,AM.BLK,AM.BK,AM.BA,AM.BKNG,AM.BWA,AM.BXP,AM.BSX,AM.BMY,AM.AVGO,AM.BR,AM.BRO,AM.BF.B,AM.CHRW,AM.CDNS,AM.CZR,AM.CPB,AM.COF,AM.CAH,AM.KMX,AM.CCL,AM.CARR,AM.CTLT,AM.CAT,AM.CBOE,AM.CBRE,AM.CDW,AM.CE,AM.CNC,AM.CNP,AM.CDAY,AM.CERN,AM.CF,AM.CRL,AM.SCHW,AM.CHTR,AM.CVX,AM.CMG,AM.CB,AM.CHD,AM.CI,AM.CINF,AM.CTAS,AM.CSCO,AM.C,AM.CFG,AM.CTXS,AM.CLX,AM.CME,AM.CMS,AM.KO,AM.CTSH,AM.CL,AM.CMCSA,AM.CMA,AM.CAG,AM.COP,AM.ED,AM.STZ,AM.CPRT,AM.GLW,AM.CTVA,AM.COST,AM.CTRA,AM.CCI,AM.CSX,AM.CMI,AM.CVS,AM.DHI,AM.DHR,AM.DRI,AM.DVA,AM.DE,AM.DAL,AM.XRAY,AM.DVN,AM.DXCM,AM.FANG,AM.DLR,AM.DFS,AM.DISCA,AM.DISCK,AM.DISH,AM.DG,AM.DLTR,AM.D,AM.DPZ,AM.DOV,AM.DOW,AM.DTE,AM.DUK,AM.DRE,AM.DD,AM.DXC,AM.EMN,AM.ETN,AM.EBAY,AM.ECL,AM.EIX,AM.EW,AM.EA,AM.LLY,AM.EMR,AM.ENPH,AM.ETR,AM.EOG,AM.EFX,AM.EQIX,AM.EQR,AM.ESS,AM.EL,AM.ETSY,AM.RE,AM.EVRG,AM.ES,AM.EXC,AM.EXPE,AM.EXPD,AM.EXR,AM.XOM,AM.FFIV,AM.FB,AM.FAST,AM.FRT,AM.FDX,AM.FIS,AM.FITB,AM.FRC,AM.FE,AM.FISV,AM.FLT,AM.FMC,AM.F,AM.FTNT,AM.FTV,AM.FBHS,AM.FOXA,AM.FOX,AM.BEN,AM.FCX,AM.GPS,AM.GRMN,AM.IT,AM.GNRC,AM.GD,AM.GE,AM.GIS,AM.GM,AM.GPC,AM.GILD,AM.GPN,AM.GL,AM.GS,AM.HAL,AM.HBI,AM.HAS,AM.HCA,AM.PEAK,AM.HSIC,AM.HES,AM.HPE,AM.HLT,AM.HOLX,AM.HD,AM.HON,AM.HRL,AM.HST,AM.HWM,AM.HPQ,AM.HUM,AM.HBAN,AM.HII,AM.IBM,AM.IEX,AM.IDXX,AM.INFO,AM.ITW,AM.ILMN,AM.INCY,AM.IR,AM.INTC,AM.ICE,AM.IFF,AM.IP,AM.IPG,AM.INTU,AM.ISRG,AM.IVZ,AM.IPGP,AM.IQV,AM.IRM,AM.JBHT,AM.JKHY,AM.J,AM.SJM,AM.JNJ,AM.JCI,AM.JPM,AM.JNPR,AM.KSU,AM.K,AM.KEY,AM.KEYS,AM.KMB,AM.KIM,AM.KMI,AM.KLAC,AM.KHC,AM.KR,AM.LHX,AM.LH,AM.LRCX,AM.LW,AM.LVS,AM.LEG,AM.LDOS,AM.LEN,AM.LNC,AM.LIN,AM.LYV,AM.LKQ,AM.LMT,AM.L,AM.LOW,AM.LUMN,AM.LYB,AM.MTB,AM.MRO,AM.MPC,AM.MKTX,AM.MAR,AM.MMC,AM.MLM,AM.MAS,AM.MA,AM.MTCH,AM.MKC,AM.MCD,AM.MCK,AM.MDT,AM.MRK,AM.MET,AM.MTD,AM.MGM,AM.MCHP,AM.MU,AM.MSFT,AM.MAA,AM.MRNA,AM.MHK,AM.TAP,AM.MDLZ,AM.MPWR,AM.MNST,AM.MCO,AM.MS,AM.MSI,AM.MSCI,AM.NDAQ,AM.NTAP,AM.NFLX,AM.NWL,AM.NEM,AM.NWSA,AM.NWS,AM.NEE,AM.NLSN,AM.NKE,AM.NI,AM.NSC,AM.NTRS,AM.NOC,AM.NLOK,AM.NCLH,AM.NRG,AM.NUE,AM.NVDA,AM.NVR,AM.NXPI,AM.ORLY,AM.OXY,AM.ODFL,AM.OMC,AM.OKE,AM.ORCL,AM.OGN,AM.OTIS,AM.PCAR,AM.PKG,AM.PH,AM.PAYX,AM.PAYC,AM.PYPL,AM.PENN,AM.PNR,AM.PBCT,AM.PEP,AM.PKI,AM.PFE,AM.PM,AM.PSX,AM.PNW,AM.PXD,AM.PNC,AM.POOL,AM.PPG,AM.PPL,AM.PFG,AM.PG,AM.PGR,AM.PLD,AM.PRU,AM.PTC,AM.PEG,AM.PSA,AM.PHM,AM.PVH,AM.QRVO,AM.QCOM,AM.PWR,AM.DGX,AM.RL,AM.RJF,AM.RTX,AM.O,AM.REG,AM.REGN,AM.RF,AM.RSG,AM.RMD,AM.RHI,AM.ROK,AM.ROL,AM.ROP,AM.ROST,AM.RCL,AM.SPGI,AM.CRM,AM.SBAC,AM.SLB,AM.STX,AM.SEE,AM.SRE,AM.NOW,AM.SHW,AM.SPG,AM.SWKS,AM.SNA,AM.SO,AM.LUV,AM.SWK,AM.SBUX,AM.STT,AM.STE,AM.SYK,AM.SIVB,AM.SYF,AM.SNPS,AM.SYY,AM.TMUS,AM.TROW,AM.TTWO,AM.TPR,AM.TGT,AM.TEL,AM.TDY,AM.TFX,AM.TER,AM.TSLA,AM.TXN,AM.TXT,AM.COO,AM.HIG,AM.HSY,AM.MOS,AM.TRV,AM.DIS,AM.TMO,AM.TJX,AM.TSCO,AM.TT,AM.TDG,AM.TRMB,AM.TFC,AM.TWTR,AM.TYL,AM.TSN,AM.USB,AM.UDR,AM.ULTA,AM.UAA,AM.UA,AM.UNP,AM.UAL,AM.UPS,AM.URI,AM.UNH,AM.UHS,AM.VLO,AM.VTR,AM.VRSN,AM.VRSK,AM.VZ,AM.VRTX,AM.VFC,AM.VIAC,AM.VTRS,AM.V,AM.VNO,AM.VMC,AM.WRB,AM.GWW,AM.WAB,AM.WBA,AM.WMT,AM.WM,AM.WAT,AM.WEC,AM.WFC,AM.WELL,AM.WST,AM.WDC,AM.WU,AM.WRK,AM.WY,AM.WHR,AM.WMB,AM.WLTW,AM.WYNN,AM.XEL,AM.XLNX,AM.XYL,AM.YUM,AM.ZBRA,AM.ZBH,AM.ZION,AM.ZTS"]

    )

def handle_msg(msgs: List[WebSocketMessage]):

    with open("C:/Users/*USERNAME*/Desktop/TradingBot/filestream.txt", "a") as file:
        for m in msgs:
            file.write(str(m) + "\n")

def main():
    c.run(handle_msg)


main()