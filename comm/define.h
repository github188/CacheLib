
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
// �迹�����
#define MAX_IMPEDANCE_VALUE   100

#define ONEPAGEROWNUMBER   10

namespace Calculation{

//const double M_PI				= 3.1415927;
const double PI_D180			= 0.01745329;			// M_PI/180

#ifdef _DEBUG
#define ASSERT(f)
#define ASSERT0()
#define ASSERT2(f, s)
#define ASSERT3(f,s1,s2)	//��Ҫ�޸ģ���ʱ����Щ����ɾ��
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

#define MAX_STR					256			// ����ַ�������

#define RUN_MODE_MAX_STR		"1"		// ������з�ʽ
#define RUN_MODE_MIN_STR		"2"		// ��С���з�ʽ
#define RUN_MODE_TEMP_STR		"3"		// ��ʱ���з�ʽ
#define NO_FIND_ELE				"δ֪"

#define	EARTH_Yn			0			//	���νӵؽ���	0
#define	EARTH_Y				1			//	���ν���		1
#define	EARTH_D				2			//	�����ν���		2

//�����ݴ�����
enum StoreType
{
    STORE_BORDER
};
//�������ݵķ�ʽ
enum
{
    CALC_WHOLE,
    CALC_SUBAREA,
    CALC_SUBAREA_WITHBORDER,
    CALC_SUBAREA_WITHBORDER_ONE
};

//У������
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
    RES_ZERO,//����
    RES_POS,//����
    RES_NEG,//ؓ��
    RES_ZERO_PU,//����
    RES_POS_PU,//����
    RES_NEG_PU//ؓ��
};
enum EmRunWay
{
    RUNWAY_INVALID = -1,
    RUNWAY_MASK = 0xff,
    RUNWAY_BIG = 0,
    RUNWAY_SMALL,
    RUNWAY_TMP,

    //�����u���������A��
    RUNWAY_EST = 0x0100,//�����u��
    RUNWAY_WARN = 0x0200//�����A��
    //
};
// ���ݽṹ���� �� �����嶨��

// Ԫ�����Ͷ���ö��( Form++ ͼ�β���)
enum SHAPE_TYPE
{
    SHAPE_ERROR					= 0,		// ��������
    SHAPE_TRANSFORMER_2			= 1,		// �����ѹ��
    SHAPE_TRANSFORMER_3			= 2,		// �����ѹ�� ���ѡ��ľ�
    SHAPE_GENERATOR				= 3,		// �����
    SHAPE_CAPACITANCE			= 4,		// ������
    SHAPE_REACTANCE				= 5,		// �翹��
    SHAPE_SPLIT_REACTANCE		= 6,		// ���ѵ翹��
    SHAPE_SYSTEM				= 7,		// ϵͳ
    SHAPE_BREAKER				= 8,		// ��·��
    SHAPE_SWITCH				= 9,		// ��բ
    SHAPE_CONNECT_LINE			= 10,		// ����
    SHAPE_TRANS_SELF			= 11,		// �����
    SHAPE_PARA_CAPACITOR		= 12,		// ����������
    SHAPE_EQUAL_LINE			= 13,		// ��ֵ��
    SHAPE_MOTOR					= 14,		// �綯��
    SHAPE_LOAD					= 15,		// ����
    SHAPE_BUS_HORIZONTAL		= 20,		// ����ĸ��
    SHAPE_BUS_VERTICAL			= 21,		// ����ĸ��
    SHAPE_LINE					= 22,		// �����·? ��·
    SHAPE_TRANSFORMER_4			= 23,		// �ľ��ѹ��
    SHAPE_SPLIT_TRANSFORMER		= 24,		// ���ѱ�ѹ��
    SHAPE_SPLIT_TRANSFORMER4	= 25,		// ������������ѱ�ѹ��
    SHAPE_T_NODE				= 26,		// T�ӵ�
    SHAPE_STATIION				= 50,		// ��ͼ
    SHAPE_LINK_LINE				= 91		// ֱ����
};
/// <summary>
/// Ԫ��������������
/// </summary>
enum PHYSICS_TYPE
{
    /// ��������
    PHYSICS_ERROR = 1000,
    /// �����·
    PHYSICS_LINE = 0,
    /// �����ѹ��
    PHYSICS_TRANSFORMER_2 = 1,
    /// �����ѹ��
    PHYSICS_TRANSFORMER_3 = 2,
    /// �����
    PHYSICS_SYNCHRONOUS_MACHINE = 3,
    /// ������
    PHYSICS_CAPACITANCE = 4,
    /// �翹��
    PHYSICS_REACTANCE = 5,
    /// ���ѵ翹��
    PHYSICS_SPLIT_REACTANCE = 6,
    /// ��ֵϵͳ
    PHYSICS_EQUAL_SOURCE = 7,
    /// ��·��
    PHYSICS_BREAKER = 8,
    /// ��բ
    PHYSICS_SWITCH = 9,
    /// ����
    PHYSICS_CONNECT_LINE = 10,
    /// �����
    PHYSICS_TRANS_SELF = 11,
    /// ����������
    PHYSICS_CAPACITOR = 12,
    /// ��ֵ��
    PHYSICS_EQUAL_LINE = 13,
    /// �綯��
    PHYSICS_ELECTROMOTOR = 14,
    /// ����
    PHYSICS_LOAD = 15,
    /// ĸ��
    PHYSICS_BUS = 20,
    /// ���ѱ�ѹ��
    PHYSICS_SPLIT_TRANSFORMER = 24,
    /// ��ѹ��
    PHYSICS_POWERTRANSFORMER = 25,
    /// ���أ���·���͵�բ��
    PHYSICS_ONOFF = 26,
    /// �翹�� �����ѡ������ѣ�
    PHYSICS_REACTANCEALL = 27,
    /// ����������������ͨ��
    PHYSICS_CAPACITANCEALL = 29,

    /// ��������
    PHYSICS_NEW_LINE = 28,

    ///  ������Ȧ
    PHYSICS_ArcSuppressioncOil = 40,
    /// �ֶ�����
    PHYSICS_HandBreaker = 41,
    /// �ֶ���բ
    PHYSICS_HandSwitch = 42,
    /// �ӵ���  GroundWire_43	43
    PHYSICS_GROUND = 43,
    ///  �۶���
    PHYSICS_Fuse = 44,
    /// �ӵ�բ��
    PHYSICS_EarthSwitch = 45,
    /// �غ�բ
    PHYSICS_AutoReclosing = 46,
    /// ������
    PHYSICS_Arrester = 47,
    /// ������
    PHYSICS_StationScreen = 48,
    /// ��·ѹ��
    PHYSICS_LINEPT = 51,
    /// ����ĸ��ѹ��
    PHYSICS_TRANSFORMERPT_3 = 52,
    /// �ľ�ĸ��ѹ��
    PHYSICS_TRANSFORMERPT_4 = 53,
    /// ��Ȧ����
    PHYSICS_CT_3 = 54,
    /// ��Ȧ����
    PHYSICS_CT_6 = 55,
    /// CT
    PHYSICS_CT = 56,
    /// PT
    PHYSICS_PT = 57,
    /// �ǿ����豸
    PHYSICS_UNOFF = 200,
    /// T�ڵ�
    PHYSICS_T_NODE = 1030

};

enum FindByMode
{
    FindMode_ByBus = 1,
    FindMode_ByNode = 2
};


enum SHAPE_STATE
{
    SHAPE_STATE_OFF,					// Ԫ��״̬�Ͽ�
    SHAPE_STATE_ON						// Ԫ��״̬�պ�
};

// Ԫ�����Ͷ���ö��(���㲿��)
enum ELEMENT_TYPE
{

    ELEMENT_LINE					= 0,		// �����·
    ELEMENT_TRANSFORMER_2			= 1,		// �����ѹ��
    ELEMENT_TRANSFORMER_3			= 2,		// �����ѹ�� ���ѡ��ľ�
    ELEMENT_SYNCHRONOUS_MACHINE	= 3,		// �����
    ELEMENT_CAPACITANCE			= 4,		// ������
    ELEMENT_REACTANCE				= 5,		// �翹��
    ELEMENT_SPLIT_REACTANCE		= 6,		// ���ѵ翹��
    ELEMENT_EQUAL_SOURCE			= 7,		// ��ֵϵͳ //�ⲿϵͳ
    ELEMENT_BREAKER				= 8,		// ��·��
    ELEMENT_SWITCH				= 9,		// ��բ
    ELEMENT_CONNECT_LINE			= 10,		// ����
    ELEMENT_TRANS_SELF			= 11,		// �����
    ELEMENT_CAPACITOR				= 12,		// ����������
    ELEMENT_EQUAL_LINE			= 13,		// ��ֵ��
    ELEMENT_ELECTROMOTOR			= 14,		// �綯��
    ELEMENT_LOAD					= 15,		// ����
    ELEMENT_BUS					= 20,		// ĸ��

    ELEMENT_SPLIT_TRANSFORMER = 24,/// ���ѱ�ѹ��
    ELEMENT_POWERTRANSFORMER      = 25,       // ��ѹ��
    ELEMENT_ONOFF					= 26,		// ���أ���·���͵�բ��
    ELEMENT_REACTANCEALL			= 27,		// �翹�������ѡ������ѣ�
    ELEMENT_NEW_LINE				= 28,	// ��������
    ELEMENT_CAPACITANCEALL		= 29,		// ����������������ͨ��
    ELEMENT_LINE_BCH              = 30,       // ��·�ֲ�����֧·


    ELEMENT_ArcSuppressioncOil = 40,///  ������Ȧ
    ELEMENT_HandBreaker = 41,/// �ֶ�����
    ELEMENT_HandSwitch = 42,/// �ֶ���բ
    ELEMENT_GROUND = 43,/// �ӵ���  GroundWire_43	43
    ELEMENT_Fuse = 44,///  �۶���
    ELEMENT_EarthSwitch = 45,/// �ӵ�բ��
    ELEMENT_AutoReclosing = 46,/// �غ�բ
    ELEMENT_Arrester = 47,/// ������
    ELEMENT_StationScreen = 48,/// ������
    ELEMENT_GroundClearance = 49,/// ��϶�ӵ�
    //ELEMENT_LineConnector=50,///// ���
    ELEMENT_TRANSFORMERPT_3 = 52,/// ����ĸ��ѹ��
    ELEMENT_TRANSFORMERPT_4 = 53,/// �ľ�ĸ��ѹ��
    ELEMENT_CT_3 = 54,/// ��Ȧ����
    ELEMENT_CT_6 = 55,/// ��Ȧ����
    ELEMENT_CT = 56,/// CT
    ELEMENT_LINEPT = 57,/// ��·ѹ��  ��51����Ϊ57
    ELEMENT_HandSwitchPlugs = 58,/// С����ͷ
    ELEMENT_PT = 59,/// PT   ��57����Ϊ59
    ELEMENT_UNOFF = 200,/// �ǿ����豸

    ELEMENT_STATION = 300,/// ���վ
    ELEMENT_ERROR = 1000,		// ��������
    ELEMENT_T_NODE = 1030/// T�ڵ�
};


//	���з�ʽ
enum EmRunMode
{
    RUN_MODE_MIN		= 0,				//	��С���з�ʽ
    RUN_MODE_MAX		= 1,				//	������з�ʽ
    RUN_MODE_CUSTOMER	= 2,				//	�Զ������з�ʽ  //grmQQNew ������֮��ɾ��
    RUN_MODE_Source		= 3,				//	�л�Դ��Ӧ�ķ�ʽ
    RUN_MODE_Begin		= 4,				//	��ʼ��ʽ
    RUN_MODE_OverHaul	= 5	,				//	���޶�Ӧ�ķ�ʽ
    RUN_MODE_Initial =6,
    RUN_MODE_Original = 7
};

//��������
enum EmBreakType
{
    TYPE_BREAKER = 0,
    TYPE_SWITCH
};

/// ��·����
/// </summary>
enum EmBranchType
{
    /// �����·
    BranchType_Line = 0,
    /// �����ѹ��
    BranchType_Transformer2 = 1,
    /// �����ѹ�� ���ѡ��ľ�
    BranchType_Transformer3 = 2,
    /// �����
    BranchType_Generator = 3,
    /// ������
    BranchType_Capacitance = 4,
    /// �翹��
    BranchType_Reactance = 5,
    /// ���ѵ翹��
    BranchType_SplitReactance = 6,
    /// ϵͳ
    BranchType_System = 7,
    /// ��·��
    BranchType_Breaker = 8,
    /// ��բ
    BranchType_Switch = 9,
    /// ����
    BranchType_ConnectLine = 10,
    /// �����
    BranchType_TransSelf = 11,
    /// ����������
    BranchType_ParaCapacitor = 12,
    /// ��ֵ��
    BranchType_Equalline = 13,
    /// �綯��
    BranchType_Motor = 14,
    /// ����
    BranchType_Load = 15,
    /// ĸ��
    BranchType_Bus = 20,
    /// ��ѹ��
    BranchType_PowerTransformer = 25,
    /// ����
    BranchType_ONOFF = 26,
    /// ���е翹��
    BranchType_REACTANCEALL = 27,
    /// T�ӵ�
    BranchType_TNode = 1030
};

enum ELEMENT_STATE
{
    ELEMENT_STATE_OFF =0,					// Ԫ��״̬�Ͽ�
    ELEMENT_STATE_ON	=1,				// Ԫ��״̬�պ�
    ELEMENT_STATE_MaxMin = 2			// ���߷�ʽ����Ķ��壬���ݴ�С��ʽ��ȡ
};
// ���ϼ����õĹ�������
enum EmFaultCalType
{
    ConstShortThree		= 0,			// ��������·
    ConstShortTwo		= 1,			// ��������·
    ConstOneToEarth		= 2,			// ����ӵض�·
    ConstTwoToEarth		= 3,			// ����ӵض�·
    ConstBreakOne		= 4,			// �����·
    ConstBreakTwo		= 5,			// �����·
    ConstThreeToEarth	= 6,			// ����ӵض�·
    ConstError			= -1			// ����ӵض�·
};

// �����İ�װλ�ã��������۵ĵ�����һ�ˣ�
enum EmProlocation
{
    Proloc_Begin			= 0,			// ����Ԫ����ʼ��
    Proloc_End			= 1,			// ����Ԫ����ĩ��
    Proloc_High			= 2,			// ��ѹ��
    Proloc_Middle		= 3,			// ��ѹ��
    Proloc_Low			= 4,			// ��ѹ��
    // grm add 2003.6.21
    Proloc_CommWires		= 5,			// ����乫������
    Proloc_CentrePoint	= 6,			// ���Ե�


    Proloc_BusPoint = 10,/// ĸ��λ��
    Proloc_BusBreakerPoint = 11,/// ĸ������λ��


    Proloc_CalBus		= 10,			// ����ĸ�߲����
    Proloc_CalBusLink	= 11,			// ����ĸ��

    Proloc_Error		= -1
    // end add
};
// Ѱ��֧·��ѡ��
enum EmFindBranchType
{
    FindBranch_All		= 0,			// Ѱ�����е�֧·
    FindBranch_Line		= 1,			// Ѱ�����е���·
    FindBranch_Trans		= 2,			// Ѱ�����еı�ѹ��
    FindBranch_Source		= 3,			// Ѱ�����е�Դ�����������ֵϵͳ��
    FindBranch_NotSource	= 4,			// Ѱ�����еķ�Դ�����������ֵϵͳ��
    // grm2004-3-15
    FindBranch_Motor		= 5,				// Ѱ�ҵ綯��
    FindBranch_LineAndTrans = 6,
    
    FindBranch_Error = -1/// ������ʹ��.
};

//���ù����õĹ�������
enum EmFaultType
{
    ConstNotEarth	= 0,				// ����·
    ConstEarth		= 1,				// �ӵض�·
    ConstBreak		= 2					// ����
};

// ��������
enum EmValueType
{
    ConstPhaseA		= 0,	// A��
    ConstPhaseB		= 1,	// B��
    ConstPhaseC		= 2,	// C��
    ConstPosi		= 3,	// ����
    ConstNega		= 4,	// ����
    ConstZero		= 5,	// ����
    ConstThreeZero	= 6,	// ��������
    // grm2005-10
    ConstAll		= 7		// �������㣬A��\B��\C��
};
// ������ʾ����
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

    OutletBus		= 6,     //��5��Ϊ6��Ϊ����ɰ汾����һ��
    IzdMax			= 7,	//'�ƶ������ѡ�ø���������ֵ max��|Ih|,|Im|,|Il|��
    IzdHalfofSum	= 8,	// '�ƶ������ѡ�ø���������ֵ0.5(|Ih|+|Im|+|Il|)
    IzdThird		= 9,	// '����: ||(Ih+Im+Il)|-(|Ih|+|Im|+|Il|)|   ����: 0.5|Ih-Il|*/
    /*OutletBus = 6 ' ���ڹ��� grm2007-5-16
        IzdMax = 7   '�ƶ������ѡ�ø���������ֵ max��|Ih|,|Im|,|Il|��
        IzdHalfofSum = 8 '�ƶ������ѡ�ø���������ֵ0.5(|Ih|+|Im|+|Il|)
        IzdThird = 9 '����: ||(Ih+Im+Il)|-(|Ih|+|Im|+|Il|)|   ����: 0.5|Ih-Il|*/

    IsBus		    = 10,    // ĸ�߲��������
    IsBusOut		= 11,    // ĸ�߳��߹���
    SelfBranchConsiderR =12,//��֧·ĩ�˹��ϣ����ǹ��ɵ���
    SelfLineEnd = 13,//��֧·ĩ�˹���
    NotAllPhase		= 20  //��ȫ�ࡡ�����ݹ�������ʶ�𣬲��øõ������ͱ�ʾ��grm2008����֧·��ȫ�ࡡ����·ĩ��λ������ʾ��SelfBus
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
//// ������ź͵����г���ص���Ϣ
//struct TagCutOneEnd
//{
//    bool			bIsOneEndCutState;		// �ǵ��˶Ͽ�״̬
//    int				nNode_i;				// �����˶Ͽ���֧·�ĶϿ���ڵ��
//    int				nNode_j;				// �����˶Ͽ���֧·�ĶϿ���Բ�ڵ��
//    string			sBranchName;			// �����˶Ͽ���֧·����
//    double			fPerCent;				// �г��㵽�г��˵�ľ���
//    EmProlocation	whichEnd;			    // �г�������һ��
//};
// ������źͼ��޻�����·��ص���Ϣ
//struct TagCheckMutualLine
//{
//    bool		bIsCheckMutualLine;			// �л�����·������
//    int			nNode_i;						// �����޵Ļ�����·����ʼ�ڵ��
//    int			nNode_j;						// �����޵Ļ�����·����ֹ�ڵ��
//    string		branchName;					// �����޵Ļ�����·������
//};

// zmx add 2002.7.17
//struct TagStopDoubleLine	// ��������ϻ�ͣ�˵���Ϣ
//{
//    bool	bSingleLineStopState;   // ͣ��һ��
//    bool	bDoubleLineStopState;   // ͣ������
//    string	sLine1;	// ͣ�˵���·1
//    string	sLine2;	// ͣ�˵���·2
//};

//֧·�翹������
enum EmMyBranchReactance
{
    ConstOneOne = 1 ,//������ʼ��翹��ʼ����
    ConstOneTwo = 2 ,//������ʼ��翹��ĩ����
    ConstTwoOne = 3 ,//������ĩ��翹��ʼ����
    ConstTwoTwo = 4 ,//������ĩ��翹��ĩ����
    ConstThirdOne = 5 ,//��ѹ����ѹ����翹��ʼ����
    ConstThirdTwo = 6//��ѹ����ѹ����翹��ĩ����

};


//֧·����������yl
enum EmMyBranchCapacitance
{
    ConstOneOneCapacitance = 1 ,//������ʼ�������ʼ����
    ConstOneOneOneCapacitance=7,
    ConstOneOneTwoCapacitance=8,
    ConstOneTwoCapacitance = 2 ,//������ʼ�������ĩ����
    ConstOneTwoOneCapacitance=9,
    ConstOneTwoTwoCapacitance=10,
    ConstTwoOneCapacitance = 3 ,//������ĩ�������ʼ����
    ConstTwoOneOneCapacitance=11,
    ConstTwoOneTwoCapacitance=12,
    ConstTwoTwoCapacitance = 4 ,//������ĩ�������ĩ����
    ConstTwoTwoOneCapacitance=13,
    ConstTwoTwoTwoCapacitance=14,
    ConstOneOneOtherCAPACITANCE=15,//��·���඼��������
    ConstOneTwoOtherCAPACITANCE=16,
    ConstTwoOneOtherCAPACITANCE=17,//��·���඼��������
    ConstTwoTwoOtherCAPACITANCE=18,
    ConstThirdOneCapacitance = 5 ,//��ѹ����ѹ���������ʼ����
    ConstThirdTwoCapacitance = 6//��ѹ����ѹ���������ĩ����
};
enum QSbus
{
    QSB_mRID=0,
    QSB_Name=1,
    QSB_GRSName=2,
    QSB_V=3,
    QSB_Ang=4,
    QSB_node=5,
    //������Ľ��
    QSB_BusOutILoad_Online=6,//ĸ�߳�����󸺺ɵ���
    QSB_BusDifferentialCurrent=7,//ĸ�߲����
    QSB_BusBrakingCurrent=8//ĸ���ƶ�����
};
}
#endif


