
//#pragma once
//#ifndef _HEADERFILE_WINDOWS_H_
//#define _HEADERFILE_WINDOWS_H_
//#include<windows.h>
//#endif


#ifndef _H_DEFINE_H_
#define _H_DEFINE_H_

#include "assert.h"

#define UNUSED(x) (void)x

#ifndef IN
#define IN
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef OUT
#define OUTemFaultType
#endif

#include <string>
#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>
using namespace std;
typedef unsigned char byte;
typedef long Long;
typedef unsigned long long ulong64;
typedef short int16;

#define FPP_MIN 0.0000001
#define DPP_MIN 0.000000001
// 阻抗无穷大
#define MAX_IMPEDANCE_VALUE   100

#define ONEPAGEROWNUMBER   10

namespace Calculation{

//const double M_PI				= 3.1415927;
const double PI_D180			= 0.01745329;			// M_PI/180

#ifdef _DEBUG
#define ASSERT(f)
#define ASSERT0()
#define ASSERT2(f, s)
#define ASSERT3(f,s1,s2)	//需要修改，暂时把这些断言删除
//#define ASSERT(f)			System::Diagnostics::Debug::Assert( f );
//#define ASSERT0()			System::Diagnostics::Debug::Assert( false );
//#define ASSERT2(f, s)		System::Diagnostics::Debug::Assert( f, s );
//#define ASSERT3(f,s1,s2)	System::Diagnostics::Debug::Assert( f, s1, s2 );
#else
#define ASSERT(f)
#define ASSERT0()
#define ASSERT2(f, s)
#define ASSERT3(f,s1,s2)
#endif

//const double ConstMaxValue	= 10000.0;  //99999999999.9;  grm2007-3
//const double ConstMinValue	= 0.0000001; //0.0001; grm2009-7 0.0000001

const double ConstMinDelta	= 0.001;
const double SQRT_3			= 1.7320508;			// Sqrt(3)
const double SQRT_3D2		= 0.86602539;			// Sqrt(3)/2
const double I_SQRT_3		= 0.57735027;			// 1/Sqrt(3)
const double I_3				= 0.33333333;			// 1/3.0f

const string emptyChar = "";
const complex<double> emptyComp(0,0);

//extern double ConstMaxValue;
//extern double ConstMinValue;
class MaxValueExp
{
public:
    static double ConstMaxValue;
    static double ConstMinValue;
};

#define MAX_STR					256			// 最大字符串长度

#define RUN_MODE_MAX_STR		"1"		// 最大运行方式
#define RUN_MODE_MIN_STR		"2"		// 最小运行方式
#define RUN_MODE_TEMP_STR		"3"		// 临时运行方式
#define NO_FIND_ELE				"未知"

#define	EARTH_Yn			0			//	星形接地接线	0
#define	EARTH_Y				1			//	星形接线		1
#define	EARTH_D				2			//	三角形接线		2

//数据暂存类型
enum StoreType
{
    STORE_BORDER
};
//加载数据的方式
enum
{
    CALC_WHOLE,
    CALC_SUBAREA,
    CALC_SUBAREA_WITHBORDER,
    CALC_SUBAREA_WITHBORDER_ONE
};

//校核类型
enum CheckType
{
    CHECKTYPE_BUS = 0,
    CHECKTYPE_LINE,
    CHECKTYPE_TRANS,
    CHECKTYPE_GENER,
    CHECKTYPE_TRANShW,
    CHECKTYPE_MAX
};

enum
{
    RES_ZERO,//零序
    RES_POS,//正序
    RES_NEG,//負序
    RES_ZERO_PU,//零序
    RES_POS_PU,//正序
    RES_NEG_PU//負序
};
enum EmRunWay
{
    RUNWAY_INVALID = -1,
    RUNWAY_MASK = 0xff,
    RUNWAY_BIG = 0,
    RUNWAY_SMALL,
    RUNWAY_TMP,

    //在线評估　在线預警
    RUNWAY_EST = 0x0100,//在线評估
    RUNWAY_WARN = 0x0200//在线預警
    //
};
// 数据结构定义 和 联合体定义

// 元件类型定义枚举( Form++ 图形部分)
enum SHAPE_TYPE
{
    SHAPE_ERROR					= 0,		// 错误类型
    SHAPE_TRANSFORMER_2			= 1,		// 二卷变压器
    SHAPE_TRANSFORMER_3			= 2,		// 三卷变压器 分裂、四卷
    SHAPE_GENERATOR				= 3,		// 发电机
    SHAPE_CAPACITANCE			= 4,		// 电容器
    SHAPE_REACTANCE				= 5,		// 电抗器
    SHAPE_SPLIT_REACTANCE		= 6,		// 分裂电抗器
    SHAPE_SYSTEM				= 7,		// 系统
    SHAPE_BREAKER				= 8,		// 断路器
    SHAPE_SWITCH				= 9,		// 刀闸
    SHAPE_CONNECT_LINE			= 10,		// 连线
    SHAPE_TRANS_SELF			= 11,		// 自耦变
    SHAPE_PARA_CAPACITOR		= 12,		// 并联电容器
    SHAPE_EQUAL_LINE			= 13,		// 等值线
    SHAPE_MOTOR					= 14,		// 电动机
    SHAPE_LOAD					= 15,		// 负荷
    SHAPE_BUS_HORIZONTAL		= 20,		// 横向母线
    SHAPE_BUS_VERTICAL			= 21,		// 纵向母线
    SHAPE_LINE					= 22,		// 输电线路? 线路
    SHAPE_TRANSFORMER_4			= 23,		// 四卷变压器
    SHAPE_SPLIT_TRANSFORMER		= 24,		// 分裂变压器
    SHAPE_SPLIT_TRANSFORMER4	= 25,		// 带第四绕组分裂变压器
    SHAPE_T_NODE				= 26,		// T接点
    SHAPE_STATIION				= 50,		// 子图
    SHAPE_LINK_LINE				= 91		// 直连线
};
/// <summary>
/// 元件物理拓扑类型
/// </summary>
enum PHYSICS_TYPE
{
    /// 错误类型
    PHYSICS_ERROR = 1000,
    /// 输电线路
    PHYSICS_LINE = 0,
    /// 二卷变压器
    PHYSICS_TRANSFORMER_2 = 1,
    /// 三卷变压器
    PHYSICS_TRANSFORMER_3 = 2,
    /// 发电机
    PHYSICS_SYNCHRONOUS_MACHINE = 3,
    /// 电容器
    PHYSICS_CAPACITANCE = 4,
    /// 电抗器
    PHYSICS_REACTANCE = 5,
    /// 分裂电抗器
    PHYSICS_SPLIT_REACTANCE = 6,
    /// 等值系统
    PHYSICS_EQUAL_SOURCE = 7,
    /// 断路器
    PHYSICS_BREAKER = 8,
    /// 刀闸
    PHYSICS_SWITCH = 9,
    /// 连线
    PHYSICS_CONNECT_LINE = 10,
    /// 自耦变
    PHYSICS_TRANS_SELF = 11,
    /// 并联电容器
    PHYSICS_CAPACITOR = 12,
    /// 等值线
    PHYSICS_EQUAL_LINE = 13,
    /// 电动机
    PHYSICS_ELECTROMOTOR = 14,
    /// 负荷
    PHYSICS_LOAD = 15,
    /// 母线
    PHYSICS_BUS = 20,
    /// 分裂变压器
    PHYSICS_SPLIT_TRANSFORMER = 24,
    /// 变压器
    PHYSICS_POWERTRANSFORMER = 25,
    /// 开关（断路器和刀闸）
    PHYSICS_ONOFF = 26,
    /// 电抗器 （分裂、不分裂）
    PHYSICS_REACTANCEALL = 27,
    /// 电容器（并联、普通）
    PHYSICS_CAPACITANCEALL = 29,

    /// 新联络线
    PHYSICS_NEW_LINE = 28,

    ///  消弧线圈
    PHYSICS_ArcSuppressioncOil = 40,
    /// 手动开关
    PHYSICS_HandBreaker = 41,
    /// 手动刀闸
    PHYSICS_HandSwitch = 42,
    /// 接地类  GroundWire_43	43
    PHYSICS_GROUND = 43,
    ///  熔断器
    PHYSICS_Fuse = 44,
    /// 接地闸刀
    PHYSICS_EarthSwitch = 45,
    /// 重合闸
    PHYSICS_AutoReclosing = 46,
    /// 避雷器
    PHYSICS_Arrester = 47,
    /// 保护屏
    PHYSICS_StationScreen = 48,
    /// 线路压变
    PHYSICS_LINEPT = 51,
    /// 三卷母线压变
    PHYSICS_TRANSFORMERPT_3 = 52,
    /// 四卷母线压变
    PHYSICS_TRANSFORMERPT_4 = 53,
    /// 三圈流变
    PHYSICS_CT_3 = 54,
    /// 六圈流变
    PHYSICS_CT_6 = 55,
    /// CT
    PHYSICS_CT = 56,
    /// PT
    PHYSICS_PT = 57,
    /// 非开断设备
    PHYSICS_UNOFF = 200,
    /// T节点
    PHYSICS_T_NODE = 1030

};

enum FindByMode
{
    FindMode_ByBus = 1,
    FindMode_ByNode = 2
};


enum SHAPE_STATE
{
    SHAPE_STATE_OFF,					// 元件状态断开
    SHAPE_STATE_ON						// 元件状态闭合
};

// 元件类型定义枚举(计算部分)
enum ELEMENT_TYPE
{

    ELEMENT_LINE					= 0,		// 输电线路
    ELEMENT_TRANSFORMER_2			= 1,		// 二卷变压器
    ELEMENT_TRANSFORMER_3			= 2,		// 三卷变压器 分裂、四卷
    ELEMENT_SYNCHRONOUS_MACHINE	= 3,		// 发电机
    ELEMENT_CAPACITANCE			= 4,		// 电容器
    ELEMENT_REACTANCE				= 5,		// 电抗器
    ELEMENT_SPLIT_REACTANCE		= 6,		// 分裂电抗器
    ELEMENT_EQUAL_SOURCE			= 7,		// 等值系统 //外部系统
    ELEMENT_BREAKER				= 8,		// 断路器
    ELEMENT_SWITCH				= 9,		// 刀闸
    ELEMENT_CONNECT_LINE			= 10,		// 连线
    ELEMENT_TRANS_SELF			= 11,		// 自耦变
    ELEMENT_CAPACITOR				= 12,		// 并联电容器
    ELEMENT_EQUAL_LINE			= 13,		// 等值线
    ELEMENT_ELECTROMOTOR			= 14,		// 电动机
    ELEMENT_LOAD					= 15,		// 负荷
    ELEMENT_BUS					= 20,		// 母线

    ELEMENT_SPLIT_TRANSFORMER = 24,/// 分裂变压器
    ELEMENT_POWERTRANSFORMER      = 25,       // 变压器
    ELEMENT_ONOFF					= 26,		// 开关（断路器和刀闸）
    ELEMENT_REACTANCEALL			= 27,		// 电抗器（分裂、不分裂）
    ELEMENT_NEW_LINE				= 28,	// 新联络线
    ELEMENT_CAPACITANCEALL		= 29,		// 电容器（并联、普通）
    ELEMENT_LINE_BCH              = 30,       // 线路分布电容支路


    ELEMENT_ArcSuppressioncOil = 40,///  消弧线圈
    ELEMENT_HandBreaker = 41,/// 手动开关
    ELEMENT_HandSwitch = 42,/// 手动刀闸
    ELEMENT_GROUND = 43,/// 接地类  GroundWire_43	43
    ELEMENT_Fuse = 44,///  熔断器
    ELEMENT_EarthSwitch = 45,/// 接地闸刀
    ELEMENT_AutoReclosing = 46,/// 重合闸
    ELEMENT_Arrester = 47,/// 避雷器
    ELEMENT_StationScreen = 48,/// 保护屏
    ELEMENT_GroundClearance = 49,/// 间隙接地
    //ELEMENT_LineConnector=50,///// 令克
    ELEMENT_TRANSFORMERPT_3 = 52,/// 三卷母线压变
    ELEMENT_TRANSFORMERPT_4 = 53,/// 四卷母线压变
    ELEMENT_CT_3 = 54,/// 三圈流变
    ELEMENT_CT_6 = 55,/// 六圈流变
    ELEMENT_CT = 56,/// CT
    ELEMENT_LINEPT = 57,/// 线路压变  由51调整为57
    ELEMENT_HandSwitchPlugs = 58,/// 小车插头
    ELEMENT_PT = 59,/// PT   由57调整为59
    ELEMENT_UNOFF = 200,/// 非开断设备

    ELEMENT_STATION = 300,/// 变电站
    ELEMENT_ERROR = 1000,		// 错误类型
    ELEMENT_T_NODE = 1030/// T节点
};


//	运行方式
enum EmRunMode
{
    RUN_MODE_MIN		= 0,				//	最小运行方式
    RUN_MODE_MAX		= 1,				//	最大运行方式
    RUN_MODE_CUSTOMER	= 2,				//	自定义运行方式  //grmQQNew 调试完之后删掉
    RUN_MODE_Source		= 3,				//	切换源对应的方式
    RUN_MODE_Begin		= 4,				//	初始方式
    RUN_MODE_OverHaul	= 5	,				//	检修对应的方式
    RUN_MODE_Initial =6,
    RUN_MODE_Original = 7
};

//开关类型
enum EmBreakType
{
    TYPE_BREAKER = 0,
    TYPE_SWITCH
};

/// 线路类型
/// </summary>
enum EmBranchType
{
    /// 输电线路
    BranchType_Line = 0,
    /// 二卷变压器
    BranchType_Transformer2 = 1,
    /// 三卷变压器 分裂、四卷
    BranchType_Transformer3 = 2,
    /// 发电机
    BranchType_Generator = 3,
    /// 电容器
    BranchType_Capacitance = 4,
    /// 电抗器
    BranchType_Reactance = 5,
    /// 分裂电抗器
    BranchType_SplitReactance = 6,
    /// 系统
    BranchType_System = 7,
    /// 断路器
    BranchType_Breaker = 8,
    /// 刀闸
    BranchType_Switch = 9,
    /// 连线
    BranchType_ConnectLine = 10,
    /// 自耦变
    BranchType_TransSelf = 11,
    /// 并联电容器
    BranchType_ParaCapacitor = 12,
    /// 等值线
    BranchType_Equalline = 13,
    /// 电动机
    BranchType_Motor = 14,
    /// 负荷
    BranchType_Load = 15,
    /// 母线
    BranchType_Bus = 20,
    /// 变压器
    BranchType_PowerTransformer = 25,
    /// 开关
    BranchType_ONOFF = 26,
    /// 所有电抗器
    BranchType_REACTANCEALL = 27,
    /// T接点
    BranchType_TNode = 1030
};

enum ELEMENT_STATE
{
    ELEMENT_STATE_OFF =0,					// 元件状态断开
    ELEMENT_STATE_ON	=1,				// 元件状态闭合
    ELEMENT_STATE_MaxMin = 2			// 在线方式补充的定义，根据大小方式获取
};
// 故障计算用的故障类型
enum EmFaultCalType
{
    ConstShortThree		= 0,			// 三相相间短路
    ConstShortTwo		= 1,			// 两相相间短路
    ConstOneToEarth		= 2,			// 单相接地短路
    ConstTwoToEarth		= 3,			// 两相接地短路
    ConstBreakOne		= 4,			// 单相断路
    ConstBreakTwo		= 5,			// 两相断路
    ConstThreeToEarth	= 6,			// 三相接地短路
    ConstError			= -1			// 三相接地短路
};

// 保护的安装位置（或所讨论的点在哪一端）
enum EmProlocation
{
    Proloc_Begin			= 0,			// 两端元件的始端
    Proloc_End			= 1,			// 两端元件的末端
    Proloc_High			= 2,			// 高压侧
    Proloc_Middle		= 3,			// 中压侧
    Proloc_Low			= 4,			// 低压侧
    // grm add 2003.6.21
    Proloc_CommWires		= 5,			// 自耦变公共绕组
    Proloc_CentrePoint	= 6,			// 中性点


    Proloc_BusPoint = 10,/// 母线位置
    Proloc_BusBreakerPoint = 11,/// 母联开关位置


    Proloc_CalBus		= 10,			// 计算母线差动保护
    Proloc_CalBusLink	= 11,			// 计算母联

    Proloc_Error		= -1
    // end add
};
// 寻找支路的选项
enum EmFindBranchType
{
    FindBranch_All		= 0,			// 寻找所有的支路
    FindBranch_Line		= 1,			// 寻找所有的线路
    FindBranch_Trans		= 2,			// 寻找所有的变压器
    FindBranch_Source		= 3,			// 寻找所有的源（发电机、等值系统）
    FindBranch_NotSource	= 4,			// 寻找所有的非源（发电机、等值系统）
    // grm2004-3-15
    FindBranch_Motor		= 5,				// 寻找电动机
    FindBranch_LineAndTrans = 6,
    
    FindBranch_Error = -1/// 计算中使用.
};

//设置故障用的故障类型
enum EmFaultType
{
    ConstNotEarth	= 0,				// 相间短路
    ConstEarth		= 1,				// 接地短路
    ConstBreak		= 2					// 断相
};

// 参量类型
enum EmValueType
{
    ConstPhaseA		= 0,	// A相
    ConstPhaseB		= 1,	// B相
    ConstPhaseC		= 2,	// C相
    ConstPosi		= 3,	// 正序
    ConstNega		= 4,	// 负序
    ConstZero		= 5,	// 零序
    ConstThreeZero	= 6,	// 三倍零序
    // grm2005-10
    ConstAll		= 7		// 正、序、零，A相\B相\C相
};
// 参量表示类型
enum EmValueExpress
{
    ConstHaveName	= 0,
    ConstGoToOne	= 1
};

//grmNew
enum EmMyCalType
{
    BackBus			= 1,
    SelfBus			= 2,
    //NextBus		= 3,
    ForwardNext     = 3,
    BackNext		= 4,

    OutletBus		= 6,     //由5改为6，为了与旧版本程序一致
    IzdMax			= 7,	//'制动点电流选用各侧电流最大值 max｛|Ih|,|Im|,|Il|｝
    IzdHalfofSum	= 8,	// '制动点电流选用各侧电流最大值0.5(|Ih|+|Im|+|Il|)
    IzdThird		= 9,	// '三卷: ||(Ih+Im+Il)|-(|Ih|+|Im|+|Il|)|   两卷: 0.5|Ih-Il|*/
    /*OutletBus = 6 ' 出口故障 grm2007-5-16
        IzdMax = 7   '制动点电流选用各侧电流最大值 max｛|Ih|,|Im|,|Il|｝
        IzdHalfofSum = 8 '制动点电流选用各侧电流最大值0.5(|Ih|+|Im|+|Il|)
        IzdThird = 9 '三卷: ||(Ih+Im+Il)|-(|Ih|+|Im|+|Il|)|   两卷: 0.5|Ih-Il|*/

    IsBus		    = 10,    // 母线差动整定参数
    IsBusOut		= 11,    // 母线出线故障
    SelfBranchConsiderR =12,//本支路末端故障，考虑过渡电阻
    SelfLineEnd = 13,//本支路末端故障
    NotAllPhase		= 20  //非全相　　依据故障描述识别，不用该单独类型表示　grm2008　本支路非全相　用线路末端位置来表示　SelfBus
};

struct TransformerState
{
    ELEMENT_STATE h;
    ELEMENT_STATE m;
    ELEMENT_STATE l;
    TransformerState()
    {
        h = ELEMENT_STATE_ON;
        m = ELEMENT_STATE_ON;
        l = ELEMENT_STATE_ON;
    }
};
//// 用来存放和单端切除相关的信息
//struct TagCutOneEnd
//{
//    bool			bIsOneEndCutState;		// 是单端断开状态
//    int				nNode_i;				// 被单端断开的支路的断开侧节点号
//    int				nNode_j;				// 被单端断开的支路的断开侧对侧节点号
//    string			sBranchName;			// 被单端断开的支路名称
//    double			fPerCent;				// 切除点到切除端点的距离
//    EmProlocation	whichEnd;			    // 切除的是哪一端
//};
// 用来存放和检修互感线路相关的信息
//struct TagCheckMutualLine
//{
//    bool		bIsCheckMutualLine;			// 有互感线路被检修
//    int			nNode_i;						// 被检修的互感线路的起始节点号
//    int			nNode_j;						// 被检修的互感线路的终止节点号
//    string		branchName;					// 被检修的互感线路的名称
//};

// zmx add 2002.7.17
//struct TagStopDoubleLine	// 用来存放上回停运的信息
//{
//    bool	bSingleLineStopState;   // 停运一回
//    bool	bDoubleLineStopState;   // 停运两回
//    string	sLine1;	// 停运的线路1
//    string	sLine2;	// 停运的线路2
//};

//支路电抗器类型
enum EmMyBranchReactance
{
    ConstOneOne = 1 ,//主对象始与电抗器始相连
    ConstOneTwo = 2 ,//主对象始与电抗器末相连
    ConstTwoOne = 3 ,//主对象末与电抗器始相连
    ConstTwoTwo = 4 ,//主对象末与电抗器末相连
    ConstThirdOne = 5 ,//变压器中压侧与电抗器始相连
    ConstThirdTwo = 6//变压器中压侧与电抗器末相连

};


//支路电容器类型yl
enum EmMyBranchCapacitance
{
    ConstOneOneCapacitance = 1 ,//主对象始与电容器始相连
    ConstOneOneOneCapacitance=7,
    ConstOneOneTwoCapacitance=8,
    ConstOneTwoCapacitance = 2 ,//主对象始与电容器末相连
    ConstOneTwoOneCapacitance=9,
    ConstOneTwoTwoCapacitance=10,
    ConstTwoOneCapacitance = 3 ,//主对象末与电容器始相连
    ConstTwoOneOneCapacitance=11,
    ConstTwoOneTwoCapacitance=12,
    ConstTwoTwoCapacitance = 4 ,//主对象末与电容器末相连
    ConstTwoTwoOneCapacitance=13,
    ConstTwoTwoTwoCapacitance=14,
    ConstOneOneOtherCAPACITANCE=15,//线路两侧都连电容器
    ConstOneTwoOtherCAPACITANCE=16,
    ConstTwoOneOtherCAPACITANCE=17,//线路两侧都连电容器
    ConstTwoTwoOtherCAPACITANCE=18,
    ConstThirdOneCapacitance = 5 ,//变压器中压侧与电容器始相连
    ConstThirdTwoCapacitance = 6//变压器中压侧与电容器末相连
};
enum QSbus
{
    QSB_mRID=0,
    QSB_Name=1,
    QSB_GRSName=2,
    QSB_V=3,
    QSB_Ang=4,
    QSB_node=5,
    //计算出的结果
    QSB_BusOutILoad_Online=6,//母线出线最大负荷电流
    QSB_BusDifferentialCurrent=7,//母线差动电流
    QSB_BusBrakingCurrent=8//母线制动电流
};
}
#endif


