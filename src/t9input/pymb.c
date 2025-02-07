#include "pyinput.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ο������粢�����޸�
//ALIENTEKս��STM32������
//ƴ�����뷨 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/22
//�汾��V1.0			    
//������������ӿƼ����޹�˾  	 												    								  
//////////////////////////////////////////////////////////////////////////////////
const unsigned char eng_mb_space []={",.;!?:<>()'"};
const unsigned char eng_mb_2     []={"012abcABC"};
const unsigned char eng_mb_3    []={" 3defDEF"};
const unsigned char eng_mb_4    []={"4ghiGHI"};
const unsigned char eng_mb_5   []={"5jklJKL"};
const unsigned char eng_mb_6    []={"6mnoMNO"};
const unsigned char eng_mb_7    []={"7pqrsPQRS"};
const unsigned char eng_mb_8   []={"8tuvTUV"};
const unsigned char eng_mb_9   []={"9wxyzWXYZ"};

const py_index eng_mb_index[]=
{
    {"2","2",sizeof(eng_mb_2),(unsigned char*)eng_mb_2},
    {"3","3",sizeof(eng_mb_3),(unsigned char*)eng_mb_3},
    {"4","4",sizeof(eng_mb_4),(unsigned char*)eng_mb_4},
    {"5","5",sizeof(eng_mb_5),(unsigned char*)eng_mb_5},
    {"6","6",sizeof(eng_mb_6),(unsigned char*)eng_mb_6},
    {"7","7",sizeof(eng_mb_7),(unsigned char*)eng_mb_7},
    {"8","8",sizeof(eng_mb_8),(unsigned char*)eng_mb_8},
    {"9","9",sizeof(eng_mb_9),(unsigned char*)eng_mb_9},
    {"22","pmark",sizeof(eng_mb_space),(unsigned char*)eng_mb_space},
};

int size_of_engmbindex()
{
	return sizeof(eng_mb_index)/sizeof(eng_mb_index[0]);
}
//�������б�
const unsigned char PY_mb_space []={"��������������������������������"};
const unsigned char PY_mb_a     []={"������߹����H����"};
const unsigned char PY_mb_ai    []={"�������������������������������������������"};
const unsigned char PY_mb_an    []={"�����������������ϰ����������������"};
const unsigned char PY_mb_ang   []={"�������a"};
const unsigned char PY_mb_ao    []={"�������������°ð���������������������������"};
const unsigned char PY_mb_ba    []={"�Ѱ˰ͰְΰŰưǰȰɰʰ̰ϰаҰӰ԰���������������"};
const unsigned char PY_mb_bai   []={"�װذٰڰ۰ܰݰ޲�����"};
const unsigned char PY_mb_ban   []={"���߰������������������������"};
const unsigned char PY_mb_bang  []={"���������������������"};
const unsigned char PY_mb_bao   []={"�����������������������������������������������������"};
const unsigned char PY_mb_bei   []={"���������������������������������������������������������"};;
const unsigned char PY_mb_ben   []={"���������������"};
const unsigned char PY_mb_beng  []={"�������±ñı�ܡ���"};
const unsigned char PY_mb_bi    []={"�ȱƱǱɱʱ˱̱ͱαϱбѱұӱԱ�ذ�ֱױرٱڱ۱ܱ�������������ݩ��޵�������������������������������������������"};
const unsigned char PY_mb_bian  []={"�߱�����ޱ�������������������������������"};
const unsigned char PY_mb_biao  []={"�����������������������"};
const unsigned char PY_mb_bie   []={"�������"};
const unsigned char PY_mb_bin   []={"������������������������������"};
const unsigned char PY_mb_bing  []={"������������������������������"};
const unsigned char PY_mb_bo    []={"�ذٰ�����������������������������������������������ެ�������������������"};
const unsigned char PY_mb_bu    []={"��������������������������߲�����������"};
const unsigned char PY_mb_ca    []={"������"};
const unsigned char PY_mb_cai   []={"�ŲƲǲ²òĲȲɲʲ˲�"};
const unsigned char PY_mb_can   []={"�ͲβϲвѲҲ����������"};
const unsigned char PY_mb_cang  []={"�Բղֲײ���"};
const unsigned char PY_mb_cao   []={"�ݲٲڲ۲�ܳ�������"};
const unsigned char PY_mb_ce    []={"����޲���"};
const unsigned char PY_mb_cen   []={"����"};
const unsigned char PY_mb_ceng  []={"�������"};
const unsigned char PY_mb_cha   []={"������������ɲ���������������������"};
const unsigned char PY_mb_chai  []={"�����٭�������"};
const unsigned char PY_mb_chan  []={"���������������������������������������������"};
const unsigned char PY_mb_chang []={"�������������������������������������������������"};
const unsigned char PY_mb_chao  []={"�������������������½�������"};
const unsigned char PY_mb_che   []={"�����������������"};
const unsigned char PY_mb_chen  []={"���������������³óĳ����������������"};
const unsigned char PY_mb_cheng []={"�ɳųƳǳȳʳ˳̳ͳγϳгѳҳ�ʢة�����������������������"};
const unsigned char PY_mb_chi   []={"�Գճֳ׳سٳڳ۳ܳݳ޳߳������߳������ܯ����������������������������"};
const unsigned char PY_mb_chong []={"����������������������"};
const unsigned char PY_mb_chou  []={"�������������ٱ������"};
const unsigned char PY_mb_chu   []={"����������������������������������ءۻ�������������������"};
const unsigned char PY_mb_chuai []={"����������"};
const unsigned char PY_mb_chuan []={"������������������������"};
const unsigned char PY_mb_chuang[]={"����������������"};
const unsigned char PY_mb_chui  []={"��������������"};
const unsigned char PY_mb_chun  []={"��������������ݻ����"};
const unsigned char PY_mb_chuo  []={"������������"};
const unsigned char PY_mb_ci    []={"�˴̴ʹ��Ų�ôĴŴƴǴȴɴ���������������"};
const unsigned char PY_mb_cong  []={"�ӴԴϴдѴ������������"};
const unsigned char PY_mb_cou   []={"������"};
const unsigned char PY_mb_cu    []={"�ִ״ش��������������"};
const unsigned char PY_mb_cuan  []={"�ڴ۴�����ߥ����"};
const unsigned char PY_mb_cui   []={"�ݴ޴ߴ����������������"};
const unsigned char PY_mb_cun   []={"��������"};
const unsigned char PY_mb_cuo   []={"��������������������������"};
const unsigned char PY_mb_da    []={"��������������������������"};
const unsigned char PY_mb_dai   []={"���������������������������ܤ߰߾��������"};
const unsigned char PY_mb_dan   []={"��������������������������������ʯ��������������������"};
const unsigned char PY_mb_dang  []={"����������������������"};
const unsigned char PY_mb_dao   []={"��������������������������߶���������"};
const unsigned char PY_mb_de    []={"�ĵõص��"};
const unsigned char PY_mb_dei   []={"��"};
const unsigned char PY_mb_deng  []={"�εŵƵǵȵɵʵ������������"};
const unsigned char PY_mb_di    []={"�ĵֵ׵صٵڵ۵ܵݵ̵͵εϵеѵҵӵԵյ���ص��ڮ��ۡ��ݶ��������������������"};
const unsigned char PY_mb_dia   []={"��"};
const unsigned char PY_mb_dian  []={"����������ߵ����������������������"};
const unsigned char PY_mb_diao  []={"���������������������"};
const unsigned char PY_mb_die   []={"����������������ܦ�����������"};
const unsigned char PY_mb_ding  []={"������������������������������������"};
const unsigned char PY_mb_diu   []={"����"};
const unsigned char PY_mb_dong  []={"��������������������������������"};
const unsigned char PY_mb_dou   []={"����������������������"};
const unsigned char PY_mb_du    []={"�ȶ��Ƕ��ɶ��������¶öĶŶƶ�ܶ�������������"};
const unsigned char PY_mb_duan  []={"�˶̶Ͷζ϶������"};
const unsigned char PY_mb_dui   []={"�ԶѶҶ�����"};
const unsigned char PY_mb_dun   []={"�ܶնֶ׶ضٶڶ۶��������������"};
const unsigned char PY_mb_duo   []={"���ȶ޶߶�����������������������"};
const unsigned char PY_mb_e     []={"������������������Ŷج������ݭ��������������������"};
const unsigned char PY_mb_ei    []={"��"};
const unsigned char PY_mb_en    []={"������"};
const unsigned char PY_mb_er    []={"�������������������������"};
const unsigned char PY_mb_fa    []={"��������������������"};
const unsigned char PY_mb_fan   []={"����������������������������������ެ����������"};
const unsigned char PY_mb_fang  []={"���������������·÷ķ�������������"};
const unsigned char PY_mb_fei   []={"�ƷǷȷɷʷ˷̷ͷηϷз�����������������������������������"};
const unsigned char PY_mb_fen   []={"�ַݷܷҷ޷߷�ӷԷշ׷طٷڷ��������������"};
const unsigned char PY_mb_feng  []={"����������������ٺۺ�������"};
const unsigned char PY_mb_fo    []={"��"};
const unsigned char PY_mb_fou   []={"���"};
const unsigned char PY_mb_fu    []={"���𸵸�����������������������������������������������������������������������������������������ۮܽ������ݳ����߻����������������������������������������������"};
const unsigned char PY_mb_ga    []={"���¼п�٤���������"};
const unsigned char PY_mb_gai   []={"�øĸŸƸǸȽ�ؤ���������"};
const unsigned char PY_mb_gan   []={"�ɸʸ˸̸͸θϸиѸҸ�������ߦ���������������"};
const unsigned char PY_mb_gang  []={"�Ըոָ׸ظٸڸ۸ܿ�����"};
const unsigned char PY_mb_gao   []={"��޸߸�ݸ�����غھ۬޻��������"};
const unsigned char PY_mb_ge    []={"�����Ǹ����������������������Ͽ�ت������ܪ��������������"};
const unsigned char PY_mb_gei   []={"��"};
const unsigned char PY_mb_gen   []={"����بݢ����"};
const unsigned char PY_mb_geng  []={"�����������������������"};
const unsigned char PY_mb_gong  []={"���������������������������������������"};
const unsigned char PY_mb_gou   []={"������������������ڸ��������������������"};
const unsigned char PY_mb_gu    []={"�ù����¹��������ĹŹƹǹȹɹʹ˹̹ͼ���ڬ��������������������������������������"};
const unsigned char PY_mb_gua   []={"�ιϹйѹҹ���ڴ�������"};
const unsigned char PY_mb_guai  []={"�Թչ���"};
const unsigned char PY_mb_guan  []={"�عٹڹ۹ܹݹ޹߹������ݸ������������"};
const unsigned char PY_mb_guang []={"������������"};
const unsigned char PY_mb_gui   []={"������������������Ȳ���������������������"};
const unsigned char PY_mb_gun   []={"�������������"};
const unsigned char PY_mb_guo   []={"������������������������������������"};
const unsigned char PY_mb_ha    []={"������"};
const unsigned char PY_mb_hai   []={"���Ⱥ�������������������"};
const unsigned char PY_mb_han   []={"��������������������������������������������������������"};
const unsigned char PY_mb_hang  []={"�к��������������"};
const unsigned char PY_mb_hao   []={"�úźƺ��º���尸亾��������޶�������"};
const unsigned char PY_mb_he    []={"�ǺȺɺʺ˺̺ͺκϺӺкѺҺԺպغֺ���ڭ�������������������"};
const unsigned char PY_mb_hei   []={"�ں�"};
const unsigned char PY_mb_hen   []={"�ܺۺݺ�"};
const unsigned char PY_mb_heng  []={"�ߺ����޿����"};
const unsigned char PY_mb_hong  []={"��������������ڧݦޮް����"};
const unsigned char PY_mb_hou   []={"��������ܩ��������������"};
const unsigned char PY_mb_hu    []={"���������˺��������������������������������������������������������������������������"};
const unsigned char PY_mb_hua   []={"�������������������������"};
const unsigned char PY_mb_huai  []={"������������"};
const unsigned char PY_mb_huan  []={"�����������������������»���佻�ۨۼ����ߧ���������������"};
const unsigned char PY_mb_huang []={"�λĻŻƻǻȻɻʻ˻̻ͻϻл��������������������"};
const unsigned char PY_mb_hui   []={"��ػٻڻһӻԻջֻ׻ۻܻݻ޻߻����������ڶ����ޥ��������������������"};
const unsigned char PY_mb_hun   []={"�������ڻ��������"};
const unsigned char PY_mb_huo   []={"�ͻ��������������޽߫������������"};
const unsigned char PY_mb_ji    []={"�������������ƼǸ������������������������������ȼɼʼ������������������������������������������¼üļż˼̼ͽ���ϵآؽ����٥����ܸ����ݽ��ު��ߴ�����������������������������������������������������������"};
const unsigned char PY_mb_jia   []={"�ҼӼμмѼԼռּ׼ؼټڼۼܼݼ޼�����٤ۣ��������������������������������"};
const unsigned char PY_mb_jian  []={"�������򽡼��߼����������������������������������������������������������������������������������������������������"};
const unsigned char PY_mb_jiang []={"���罩����������������������ǿ����������������"};
const unsigned char PY_mb_jiao  []={"�нϾ���У�������������������������½ýĽŽƽȽɽʽ˽̽ͽν�ٮ��ܴ����������������������"};
const unsigned char PY_mb_jie   []={"�ӽ������ҽܽԽ��ݽ޽սֽ׽ؽٽڽ۽߽�������ڦڵ�������������������������"};
const unsigned char PY_mb_jin   []={"������������������������������������ݣ������������������������"};
const unsigned char PY_mb_jing  []={"������������������������������������������������������������ݼ������������������"};
const unsigned char PY_mb_jiong []={"����������"};
const unsigned char PY_mb_jiu   []={"�;žȾƾþ��������¾ľɾʾ˾̾ξ�����������������"};
const unsigned char PY_mb_ju    []={"�޾߾���峵���ӾԾվ־׾ؾپھ۾ܾݾ���ϾоѾ���ڪ��������������������������������������������"};
const unsigned char PY_mb_juan  []={"��������Ȧ۲�������������"};
const unsigned char PY_mb_jue   []={"���������Ǿ����ž������������ܥާ���������������������"};
const unsigned char PY_mb_jun   []={"��������������������������������"};
const unsigned char PY_mb_ka    []={"��������������"};
const unsigned char PY_mb_kai   []={"�������������������������"};
const unsigned char PY_mb_kan   []={"������������٩�����ݨ�ۼ��"};
const unsigned char PY_mb_kang  []={"��������������������"};
const unsigned char PY_mb_kao   []={"����������������"};
const unsigned char PY_mb_ke    []={"�ɿʿ˿ƿ̿Ϳ�྿����¿ÿĿſǿ���������������������������������"};
const unsigned char PY_mb_ken   []={"�Ͽпѿ���"};
const unsigned char PY_mb_keng  []={"�ӿ������"};
const unsigned char PY_mb_kong  []={"�տֿ׿�������"};
const unsigned char PY_mb_kou   []={"�ٿڿۿ���ޢߵ����"};
const unsigned char PY_mb_ku    []={"�ݿ޿߿���������ܥ�"};
const unsigned char PY_mb_kua   []={"������٨�"};
const unsigned char PY_mb_kuai  []={"��������ۦ������"};
const unsigned char PY_mb_kuan  []={"�����"};
const unsigned char PY_mb_kuang []={"�����������ڲڿ������������"};
const unsigned char PY_mb_kui   []={"����������������������ظ���������������������������"};
const unsigned char PY_mb_kun   []={"��������������������"};
const unsigned char PY_mb_kuo   []={"����������"};
const unsigned char PY_mb_la    []={"�������������������������"};
const unsigned char PY_mb_lai   []={"���������������������"};
const unsigned char PY_mb_lan   []={"����������������������������������������"};
const unsigned char PY_mb_lang  []={"������������������������ݹ"};
const unsigned char PY_mb_lao   []={"�������������������������������������"};
const unsigned char PY_mb_le    []={"��������߷����"};
const unsigned char PY_mb_lei   []={"��������������������������ڳ������������"};
const unsigned char PY_mb_leng  []={"������ܨ�"};
const unsigned char PY_mb_li    []={"��������������������������������������������������������������������تٳٵ۪����ݰ��޼߿�����������������������������������������������������������"};
const unsigned char PY_mb_lia   []={"��"};
const unsigned char PY_mb_lian  []={"������������������������������������������������"};
const unsigned char PY_mb_liang []={"������������������������ܮݹ�����"};
const unsigned char PY_mb_liao  []={"��������������������������ޤ������������"};
const unsigned char PY_mb_lie   []={"�������������������������"};
const unsigned char PY_mb_lin   []={"�����������������������������������������������"};
const unsigned char PY_mb_ling  []={"��������������������������������۹��������������������"};
const unsigned char PY_mb_liu   []={"����������������������µ½����������������"};
const unsigned char PY_mb_lo    []={"��"}; 
const unsigned char PY_mb_long  []={"��Ū��������¡¢£¤���������������"};
const unsigned char PY_mb_lou   []={"¥¦§¨©ª¶�����������������"};
const unsigned char PY_mb_lu    []={"��«¬­®¯°±²³´µ¶·¸¹º»¼½¾����ߣ������������������������������������"};
const unsigned char PY_mb_lv    []={"��������������������������¿�������������"}; 
const unsigned char PY_mb_luan  []={"������������"};
const unsigned char PY_mb_lue   []={"�����"};
const unsigned char PY_mb_lun   []={"����������������"};
const unsigned char PY_mb_luo   []={"���޸����������������������������������������������������"};
const unsigned char PY_mb_m     []={"߼"};  
const unsigned char PY_mb_ma    []={"������������������ĦĨ��������"};
const unsigned char PY_mb_mai   []={"������������������áܬ��������������"};
const unsigned char PY_mb_man   []={"����������á������"};
const unsigned char PY_mb_mang  []={"âãäåæç��������"};
const unsigned char PY_mb_mao   []={"èéêëì��îïðñòó��í�����������������"};
const unsigned char PY_mb_me    []={"ô"};
const unsigned char PY_mb_mei   []={"ÿ����õö��������÷øùúûüýþ��ݮ�����������"};
const unsigned char PY_mb_men   []={"�������������"};
const unsigned char PY_mb_meng  []={"��������������������ޫ��������������"};
const unsigned char PY_mb_mi    []={"����������������������������������ڢ������������������������"};
const unsigned char PY_mb_mian  []={"�����������������������������"};
const unsigned char PY_mb_miao  []={"������������������������������"};
const unsigned char PY_mb_mie   []={"����ؿ�����"};
const unsigned char PY_mb_min   []={"������������������������������"};
const unsigned char PY_mb_ming  []={"������������ڤ���������"};
const unsigned char PY_mb_miu   []={"����"};
const unsigned char PY_mb_mo    []={"��û��ġĢģĤĥĦħĨĩĪīĬĭĮįİ���������������������������"};
const unsigned char PY_mb_mou   []={"ıĲĳٰ����������"};
const unsigned char PY_mb_mu    []={"ľĿ��ģĲĴĵĶķĸĹĺĻļĽ���������������"};
const unsigned char PY_mb_na    []={"����������������������"};
const unsigned char PY_mb_nai   []={"����������ؾ٦ܵ����"};
const unsigned char PY_mb_nan   []={"����������������"};
const unsigned char PY_mb_nang  []={"������߭��"};
const unsigned char PY_mb_nao   []={"����������ث������������"};
const unsigned char PY_mb_ne    []={"����ګ"};
const unsigned char PY_mb_nei   []={"����"};
const unsigned char PY_mb_nen   []={"���"};
const unsigned char PY_mb_neng  []={"��"};
const unsigned char PY_mb_ng    []={"��"}; 
const unsigned char PY_mb_ni    []={"������������������������٣�����������������"};
const unsigned char PY_mb_nian  []={"��������������إ���������"};
const unsigned char PY_mb_niang []={"����"};
const unsigned char PY_mb_niao  []={"������������"};
const unsigned char PY_mb_nie   []={"�������������������ؿ�����"};
const unsigned char PY_mb_nin   []={"��"};
const unsigned char PY_mb_ning  []={"��������šŢ�������"};
const unsigned char PY_mb_niu   []={"ţŤťŦ�������"};
const unsigned char PY_mb_nong  []={"ŧŨũŪٯ��"};
const unsigned char PY_mb_nou   []={"��"};
const unsigned char PY_mb_nu    []={"ūŬŭ��������"};
const unsigned char PY_mb_nuan  []={"ů"};
const unsigned char PY_mb_nue   []={"űŰ"};
const unsigned char PY_mb_nuo   []={"��ŲųŴŵ�������"};
const unsigned char PY_mb_nv    []={"Ů�����"};
const unsigned char PY_mb_o     []={"Ŷ���"};
const unsigned char PY_mb_ou    []={"ŷŸŹźŻżŽ��ک�����"};
const unsigned char PY_mb_pa    []={"�ǰ�žſ��������������"};
const unsigned char PY_mb_pai   []={"��������������ٽ����"};
const unsigned char PY_mb_pan   []={"�������������������������������������"};
const unsigned char PY_mb_pang  []={"����������������������"};
const unsigned char PY_mb_pao   []={"������������������������"};
const unsigned char PY_mb_pei   []={"�����������������������������"};
const unsigned char PY_mb_pen   []={"������"};
const unsigned char PY_mb_peng  []={"������������������������������ܡ���"};
const unsigned char PY_mb_pi    []={"�ٷ�����������������ơƢƣƤƥƦƧاƨƩ�����������ۯ����ܱ��ߨ�����������������������"};
const unsigned char PY_mb_pian  []={"���ƪƫƬƭ��������������"};
const unsigned char PY_mb_piao  []={"ƮƯưƱ����������������"};
const unsigned char PY_mb_pie   []={"ƲƳد���"};
const unsigned char PY_mb_pin   []={"ƴƵƶƷƸ�������"};
const unsigned char PY_mb_ping  []={"��ƹƺƻƼƽƾƿ����ٷ�����"};
const unsigned char PY_mb_po    []={"��������������������������۶�����������"};
const unsigned char PY_mb_pou   []={"������"};
const unsigned char PY_mb_pu    []={"�ձ������������������������������������������������"};
const unsigned char PY_mb_qi    []={"����������������������������������������������������������������������������������ؽ�����������ٹ��ܻ��������������ޭ����������������������"};
const unsigned char PY_mb_qia   []={"����ǡǢ������"};
const unsigned char PY_mb_qian  []={"ǣǤǥǦǧǨǩǪǫǬǭǮǯǰǱǲǳǴǵǶǷǸ��ٻ����ܷ����ݡ������������������������"};
const unsigned char PY_mb_qiang []={"ǿǹǺǻǼǽǾ�������������������������"};
const unsigned char PY_mb_qiao  []={"��������������������������������ȸڽ��������������������"};
const unsigned char PY_mb_qie   []={"����������٤�����ۧ������"};
const unsigned char PY_mb_qin   []={"��������������������������������������������"};
const unsigned char PY_mb_qing  []={"���������������������������������������������������"};
const unsigned char PY_mb_qiong []={"�����������������"};
const unsigned char PY_mb_qiu   []={"�������������������ٴ������������������������"};
const unsigned char PY_mb_qu    []={"ȥȡ����������������ȢȣȤ������ڰ۾����ޡ޾����������������������"};
const unsigned char PY_mb_quan  []={"ȦȧȨȩȪȫȬȭȮȯȰڹ�����������������"};
const unsigned char PY_mb_que   []={"ȱȲȳȴȵȶȷȸ�����"};
const unsigned char PY_mb_qui   []={"��"};  
const unsigned char PY_mb_qun   []={"ȹȺ����"};
const unsigned char PY_mb_ran   []={"ȻȼȽȾ������"};
const unsigned char PY_mb_rang  []={"ȿ�����������"};
const unsigned char PY_mb_rao   []={"�����������"};
const unsigned char PY_mb_re    []={"������"};
const unsigned char PY_mb_ren   []={"�����������������������������������"};
const unsigned char PY_mb_reng  []={"����"};
const unsigned char PY_mb_ri    []={"��"};
const unsigned char PY_mb_rong  []={"������������������������������"};
const unsigned char PY_mb_rou   []={"������������"};
const unsigned char PY_mb_ru    []={"���������������������������޸�������"};
const unsigned char PY_mb_ruan  []={"������"};
const unsigned char PY_mb_rui   []={"�����������ި�"};
const unsigned char PY_mb_run   []={"����"};
const unsigned char PY_mb_ruo   []={"����ټ��"};
const unsigned char PY_mb_sa    []={"������ئ�����"};
const unsigned char PY_mb_sai   []={"����������"};
const unsigned char PY_mb_san   []={"����ɡɢ�����"};
const unsigned char PY_mb_sang  []={"ɣɤɥ�����"};
const unsigned char PY_mb_sao   []={"ɦɧɨɩ����ܣ����"};
const unsigned char PY_mb_se    []={"��ɪɫɬ����"};
const unsigned char PY_mb_sen   []={"ɭ"};
const unsigned char PY_mb_seng  []={"ɮ"};
const unsigned char PY_mb_sha   []={"ɯɰɱɲɳɴɵɶɷɼ��������������"};
const unsigned char PY_mb_shai  []={"ɫɸɹ"};
const unsigned char PY_mb_shan  []={"����ɺɻɼɽɾɿ��������������������դڨ�����������۷���������������������"};
const unsigned char PY_mb_shang []={"�������������������������"};
const unsigned char PY_mb_shao  []={"������������������������ۿ�����������"};
const unsigned char PY_mb_she   []={"�����������������������������������������"};
const unsigned char PY_mb_shei  []={"˭"}; 
const unsigned char PY_mb_shen  []={"���������������������������������ʲڷ��ݷ�������������"};
const unsigned char PY_mb_sheng []={"ʤ��������������ʡʢʣʥ����������"};
const unsigned char PY_mb_shi   []={"����ʱʶ��ʵʦʮʯʳ��ʧʨʩʪʫʬʭʰʲʴʷʸʹʺʻʼʽʾʿ��������������������������������������߱��ֳ������ݪ��������������������������"};
const unsigned char PY_mb_shou  []={"���������������������������"};
const unsigned char PY_mb_shu   []={"����������������������������������������������������������������ˡٿ�������ح���������������"};
const unsigned char PY_mb_shua  []={"ˢˣ�"};
const unsigned char PY_mb_shuai []={"��ˤ˥˦˧�"};
const unsigned char PY_mb_shuan []={"˨˩����"};
const unsigned char PY_mb_shuang[]={"˪˫ˬ����"};
const unsigned char PY_mb_shui  []={"ˮ˯˰˵��"};
const unsigned char PY_mb_shun  []={"˱˲˳˴"};
const unsigned char PY_mb_shuo  []={"˵��˶˷˸����������"};
const unsigned char PY_mb_si    []={"˼����˹˺˻˽˾˿������������������ٹ���������������������������"};
const unsigned char PY_mb_song  []={"��������������������������ݿڡ"};
const unsigned char PY_mb_sou   []={"�����������������������޴"};
const unsigned char PY_mb_su    []={"�����������������������������������������"};
const unsigned char PY_mb_suan  []={"�������"};
const unsigned char PY_mb_sui   []={"����������������������"};
const unsigned char PY_mb_sun   []={"������"};
const unsigned char PY_mb_suo   []={"�����������������������ɯ��������"};
const unsigned char PY_mb_ta    []={"���������̣̤��̡̢�����������������"};
const unsigned char PY_mb_tai   []={"̧̨̥̦̩̪̫̬̭��ۢ޷����������"};
const unsigned char PY_mb_tan   []={"��̴̵̶̷̸̮̯̰̱̲̳̹̺̻̼̽̾̿��۰������������"};
const unsigned char PY_mb_tang  []={"�������������������������������������������������"};
const unsigned char PY_mb_tao   []={"����������������������߶��������ػ"};
const unsigned char PY_mb_te    []={"��߯���"};
const unsigned char PY_mb_teng  []={"����������"};
const unsigned char PY_mb_ti    []={"���������������������������������������������"};
const unsigned char PY_mb_tian  []={"����������������������������"};
const unsigned char PY_mb_tiao  []={"������������٬������������������"};
const unsigned char PY_mb_tie   []={"�����������"};
const unsigned char PY_mb_ting  []={"������ͣͤͥͦͧ͢͡��������������"};
const unsigned char PY_mb_tong  []={"ͩͫͬͭͮͯͰͱͲͳʹ١��������ͪ���������"};
const unsigned char PY_mb_tou   []={"͵Ͷͷ͸������"};
const unsigned char PY_mb_tu    []={"͹ͺͻͼͽ;Ϳ��������ܢݱ������"};
const unsigned char PY_mb_tuan  []={"���������"};
const unsigned char PY_mb_tui   []={"��������������"};
const unsigned char PY_mb_tun   []={"�����������������"};
const unsigned char PY_mb_tuo   []={"˵������������������������ر٢����������������������"};
const unsigned char PY_mb_wa    []={"�������������������"};
const unsigned char PY_mb_wai   []={"������"};
const unsigned char PY_mb_wan   []={"��������������������������������������ܹݸ��������������"};
const unsigned char PY_mb_wang  []={"�����������������������������"};
const unsigned char PY_mb_wei   []={"Ϊλν��Ρ΢ΣΤάΥΦΧΨΩΫέήίΰαβγδεζηθικμξο������������ޱ�����������������������������������"};
const unsigned char PY_mb_wen   []={"�������������������������������"};
const unsigned char PY_mb_weng  []={"��������޳"};
const unsigned char PY_mb_wo    []={"���������������������ݫ��������"};
const unsigned char PY_mb_wu    []={"�����������������������أ����������������������������������������������������������������������������������"};
const unsigned char PY_mb_xi    []={"ϣϰϦϷϸ����������������������ϡ��ϢϤϥϧϨϪϫϬϮϯϱϲϴϵ϶���������������ϭϩϳ��������ۭݾ���������������������������������������������"};
const unsigned char PY_mb_xia   []={"����ϹϺϻϼϽϾϿ�����������������������"};
const unsigned char PY_mb_xian  []={"������������������ϳϴ��������������������������������������ݲ޺������������������������������"};
const unsigned char PY_mb_xiang []={"������������������������������������������ܼ������������"};
const unsigned char PY_mb_xiao  []={"С��Ф��������������������ТУХЦЧ���������������������"};
const unsigned char PY_mb_xie   []={"ЩЪЫЬЭЮЯабвгдежзийклмѪ��ШҶ��������ޯߢ���������������������"};
const unsigned char PY_mb_xin   []={"ноп��������������ܰݷ����ض����"};
const unsigned char PY_mb_xing  []={"������������ʡ�������������������������ߩ���"};
const unsigned char PY_mb_xiong []={"��������������ܺ"};
const unsigned char PY_mb_xiu   []={"������������������������������������"};
const unsigned char PY_mb_xu    []={"��������������������������������������ڼ������ޣ������������������"};
const unsigned char PY_mb_xuan  []={"��ѡ��������������Ѥ��ѣѢ����������������������ȯ"};
const unsigned char PY_mb_xue   []={"��ѥѦѧѨѩѪ���������"};
const unsigned char PY_mb_xun   []={"ѰѮѸѶѯѵѫѬѭѱѲѳѴѷۨ����޹������࿣ݡަ����������"};
const unsigned char PY_mb_ya    []={"��ѿ����ѹ��ѺѻѼѽѾ����������������������������������"};
const unsigned char PY_mb_yan   []={"��������������������������������������������������������������������������������ٲ��Ǧ����������ڥ����۱۳ܾ�����������������������������"};
const unsigned char PY_mb_yang  []={"������������������������������������������������"};
const unsigned char PY_mb_yao   []={"Ҫ��ҧҩ����ҡҢ���ңҤҥҦҨҫԿزس��ߺ����ű�����������������������"};
const unsigned char PY_mb_ye    []={"ҲҵҳҶҹҺҬҭҮүҰұҴҷҸа��������������������"};
const unsigned char PY_mb_yi    []={"һ����������������������ҼҽҾҿ�����������������������������������������������������������������������β��������٫ڱ������ܲ����޲������߽߮�����������������������������������������������������������������"};
const unsigned char PY_mb_yin   []={"����ӡ����������������������������ط�۴����ܧ������������������"};
const unsigned char PY_mb_ying  []={"ӳӰӢӨөӪӫӣӤӥӦӧӬӭӮӯӱӲ��ݺ�����������۫�������������������"};
const unsigned char PY_mb_yo    []={"Ӵ���"};
const unsigned char PY_mb_yong  []={"��ӵӶ��ӿӷӸӹӺӻӼӽӾ����ٸ��ܭ�����������"};
const unsigned char PY_mb_you   []={"��������������������������������������������������������٧ݬݯݵ��������������������"};
const unsigned char PY_mb_yu    []={"������������������ԡ������������������������������������������������������������ԢԣԤԥԦ��ع�����εξخ��ٶ������������������������������������������������������������������������"};
const unsigned char PY_mb_yuan  []={"ԭԪ԰ԱԲԴԵԶԸԹԺԧԨԩԫԬԮԯԳԷ��ܫܾ������������������"};
const unsigned char PY_mb_yue   []={"��������˵ԻԼԽԾԿ����������������"};
const unsigned char PY_mb_yun   []={"��Ա�������������������������ܿ��۩����������"};
const unsigned char PY_mb_za    []={"��������զ����"};
const unsigned char PY_mb_zai   []={"������������������"};
const unsigned char PY_mb_zan   []={"���������������������"};
const unsigned char PY_mb_zang  []={"�����������"};
const unsigned char PY_mb_zao   []={"������������������������������"};
const unsigned char PY_mb_ze    []={"��������զ�����������������"};
const unsigned char PY_mb_zei   []={"��"};
const unsigned char PY_mb_zen   []={"����"};
const unsigned char PY_mb_zeng  []={"��������������"};
const unsigned char PY_mb_zha   []={"��բ����������ագդեզէըթ߸��������������������"};
const unsigned char PY_mb_zhai  []={"լխծ����ժիկ���"};
const unsigned char PY_mb_zhan  []={"ռսվչմ��հձղճյնշոպ��ջտ�������"};
const unsigned char PY_mb_zhang []={"���������������������������������������۵���"};
const unsigned char PY_mb_zhao  []={"����צ���׳�����گ������������������"};
const unsigned char PY_mb_zhe   []={"������������������������ߡ��������������"};
const unsigned char PY_mb_zhen  []={"��������������������������������֡����������������������������"};
const unsigned char PY_mb_zheng []={"��֤��������������������֢֣ں����������"};
const unsigned char PY_mb_zhi   []={"ֱֻ֮��ֹ֫��ֽ������֧ʶִֵּ֥֭־ְֲֳֶַָֺֿ֦֪֬֯��������������֨֩������������ش��ۤ������������������������������������������������������������"};
const unsigned char PY_mb_zhong []={"����������������������ڣ���������"};
const unsigned char PY_mb_zhou  []={"����������������������������ݧ����������������"};
const unsigned char PY_mb_zhu   []={"��סעף������������������������������������������פ������٪����ۥ��������������������������������ؼ"};
const unsigned char PY_mb_zhua  []={"ץצ��"};
const unsigned char PY_mb_zhuai []={"קת"};
const unsigned char PY_mb_zhuan []={"��רשת׫׬׭�������"};
const unsigned char PY_mb_zhuang[]={"��׮ׯװױײ׳״���"};
const unsigned char PY_mb_zhui  []={"׵׶׷׸׹׺�������"};
const unsigned char PY_mb_zhun  []={"׻׼����"};
const unsigned char PY_mb_zhuo  []={"��׽׾׿��������������پ��ߪ�����������"};
const unsigned char PY_mb_zi    []={"���������������������������������֨�������������������������������������"};
const unsigned char PY_mb_zong  []={"����������������������"};
const unsigned char PY_mb_zou   []={"������������۸����"};
const unsigned char PY_mb_zu    []={"����������������������"};
const unsigned char PY_mb_zuan  []={"��߬������"};
const unsigned char PY_mb_zui   []={"��������ީ��"};
const unsigned char PY_mb_zun   []={"��������ߤ"};
const unsigned char PY_mb_zuo   []={"�����������������������������������"};


//words
const unsigned char cy_mb_27    []={"���ϳ��˳��ⳣ��"};
const unsigned char cy_mb_89    []={"����"};
const unsigned char cy_mb_35    []={"���ַ���"};
const unsigned char cy_mb_97    []={"��ʶ"};
const unsigned char cy_mb_29    []={"��Ϊ��������Ϊ��δ"};
const unsigned char cy_mb_79    []={"������������"};
const unsigned char cy_mb_39    []={"������ѧ"};
const unsigned char cy_mb_55    []={"�����ռ�Ƽ��ɼ�����������������"};
const unsigned char cy_mb_57    []={"���ǿ�ʼ����"};
const unsigned char cy_mb_25    []={"���������������"};
const unsigned char cy_mb_59    []={"���Կ�ѧ��Ц"};
const unsigned char cy_mb_99    []={"����������Щ��Ϊ��Ϊ����"};
const unsigned char cy_mb_69    []={"û��û����������"};
const unsigned char cy_mb_95    []={"�Լ��Ѿ���������"};
const unsigned char cy_mb_262   []={"���ܱ���"};
const unsigned char cy_mb_222   []={"������Ӵ˲�γ�������"};
const unsigned char cy_mb_862   []={"������������"};
const unsigned char cy_mb_762   []={"ʲô������������˵������"};
const unsigned char cy_mb_932   []={"֪��һ���������е�һ���е�ѡ��"};
const unsigned char cy_mb_962   []={"��ô��ô����������������ֻ��֮����������"};
const unsigned char cy_mb_662   []={"������ôũ��"};
const unsigned char cy_mb_372   []={"���ǵ�ʱ���ϵ�Ȼ����"};
const unsigned char cy_mb_745   []={"����ʱ��˯��"};
const unsigned char cy_mb_432   []={"��������"};
const unsigned char cy_mb_832   []={"ͽ������"};
const unsigned char cy_mb_5429  []={"������������"};
const unsigned char cy_mb_3829  []={"����"};
const unsigned char cy_mb_7442  []={"ʱ���������"};
const unsigned char cy_mb_9829  []={"����"};
const unsigned char cy_mb_6429  []={"�Ǹ��ĸ��ܹ��ѹ�"};
const unsigned char cy_mb_9429  []={"����Ժ��й��������Ҳ��Ҳ��Ҳ��"};

const py_index cy_index[]=
{
    {"27","fl",  sizeof(cy_mb_27  ),(unsigned char*)cy_mb_27  },
    {"89","fl",  sizeof(cy_mb_89  ),(unsigned char*)cy_mb_89  },
    {"35","fl",  sizeof(cy_mb_35  ),(unsigned char*)cy_mb_35  },
    {"97","ys",  sizeof(cy_mb_97  ),(unsigned char*)cy_mb_97  },
    {"29","cw",  sizeof(cy_mb_29  ),(unsigned char*)cy_mb_29  },
    {"79","sy",  sizeof(cy_mb_79  ),(unsigned char*)cy_mb_79  },
    {"39","dx",  sizeof(cy_mb_39  ),(unsigned char*)cy_mb_39  },
    {"55","kj",  sizeof(cy_mb_55  ),(unsigned char*)cy_mb_55  },
    {"57","ao",  sizeof(cy_mb_57  ),(unsigned char*)cy_mb_57  },
    {"25","ba",  sizeof(cy_mb_25  ),(unsigned char*)cy_mb_25  },
    {"59","bi",  sizeof(cy_mb_59  ),(unsigned char*)cy_mb_59  },
    {"99","bo",  sizeof(cy_mb_99  ),(unsigned char*)cy_mb_99  },
    {"69","bu",  sizeof(cy_mb_69  ),(unsigned char*)cy_mb_69  },
    {"95","ca",  sizeof(cy_mb_95  ),(unsigned char*)cy_mb_95  },
    {"222","cc", sizeof(cy_mb_862 ),(unsigned char*)cy_mb_862 },
    {"862","ce", sizeof(cy_mb_862 ),(unsigned char*)cy_mb_862 },
    {"962","ce", sizeof(cy_mb_962 ),(unsigned char*)cy_mb_962 },
    {"762","cu", sizeof(cy_mb_762 ),(unsigned char*)cy_mb_762 },
    {"932","da", sizeof(cy_mb_932 ),(unsigned char*)cy_mb_932 },
    {"962","de", sizeof(cy_mb_962 ),(unsigned char*)cy_mb_962 },
    {"662","di", sizeof(cy_mb_662 ),(unsigned char*)cy_mb_662 },
    {"372","du", sizeof(cy_mb_372 ),(unsigned char*)cy_mb_372 },
    {"745","en", sizeof(cy_mb_745 ),(unsigned char*)cy_mb_745 },
    {"432","en", sizeof(cy_mb_432 ),(unsigned char*)cy_mb_432 },
    {"832","en", sizeof(cy_mb_832 ),(unsigned char*)cy_mb_432 },
    {"5429","er",sizeof(cy_mb_5429),(unsigned char*)cy_mb_5429},
    {"6429","er",sizeof(cy_mb_6429),(unsigned char*)cy_mb_6429},
    {"7442","er",sizeof(cy_mb_7442),(unsigned char*)cy_mb_7442},
    {"3829","er",sizeof(cy_mb_3829),(unsigned char*)cy_mb_3829},
    {"9829","er",sizeof(cy_mb_9829),(unsigned char*)cy_mb_9829},
    {"9429","er",sizeof(cy_mb_9429),(unsigned char*)cy_mb_9429},
};

int size_of_cyindex()
{
	return sizeof(cy_index)/sizeof(cy_index[0]);
}

//ƴ��������
const py_index py_index3[]=
{
{"2","pmark",sizeof(PY_mb_space),(unsigned char*)PY_mb_space},
{"2","a",sizeof(PY_mb_a),(unsigned char*)PY_mb_a},
{"3","e",sizeof(PY_mb_e),(unsigned char*)PY_mb_e},
{"6","o",sizeof(PY_mb_o),(unsigned char*)PY_mb_o},
{"24","ai",sizeof(PY_mb_ai),(unsigned char*)PY_mb_ai},
{"26","an",sizeof(PY_mb_an),(unsigned char*)PY_mb_an},
{"26","ao",sizeof(PY_mb_ao),(unsigned char*)PY_mb_ao},
{"22","ba",sizeof(PY_mb_ba),(unsigned char*)PY_mb_ba},
{"24","bi",sizeof(PY_mb_bi),(unsigned char*)PY_mb_bi},
{"26","bo",sizeof(PY_mb_bo),(unsigned char*)PY_mb_bo},
{"28","bu",sizeof(PY_mb_bu),(unsigned char*)PY_mb_bu},
{"22","ca",sizeof(PY_mb_ca),(unsigned char*)PY_mb_ca},
{"23","ce",sizeof(PY_mb_ce),(unsigned char*)PY_mb_ce},
{"24","ci",sizeof(PY_mb_ci),(unsigned char*)PY_mb_ci},
{"28","cu",sizeof(PY_mb_cu),(unsigned char*)PY_mb_cu},
{"32","da",sizeof(PY_mb_da),(unsigned char*)PY_mb_da},
{"33","de",sizeof(PY_mb_de),(unsigned char*)PY_mb_de},
{"34","di",sizeof(PY_mb_di),(unsigned char*)PY_mb_di},
{"38","du",sizeof(PY_mb_du),(unsigned char*)PY_mb_du},
{"36","en",sizeof(PY_mb_en),(unsigned char*)PY_mb_en},
{"37","er",sizeof(PY_mb_er),(unsigned char*)PY_mb_er},
{"32","fa",sizeof(PY_mb_fa),(unsigned char*)PY_mb_fa},
{"36","fo",sizeof(PY_mb_fo),(unsigned char*)PY_mb_fo},
{"38","fu",sizeof(PY_mb_fu),(unsigned char*)PY_mb_fu},
{"42","ha",sizeof(PY_mb_ha),(unsigned char*)PY_mb_ha},
{"42","ga",sizeof(PY_mb_ga),(unsigned char*)PY_mb_ga},
{"43","ge",sizeof(PY_mb_ge),(unsigned char*)PY_mb_ge},
{"43","he",sizeof(PY_mb_he),(unsigned char*)PY_mb_he},
{"48","gu",sizeof(PY_mb_gu),(unsigned char*)PY_mb_gu},
{"48","hu",sizeof(PY_mb_hu),(unsigned char*)PY_mb_hu},
{"54","ji",sizeof(PY_mb_ji),(unsigned char*)PY_mb_ji},
{"58","ju",sizeof(PY_mb_ju),(unsigned char*)PY_mb_ju},
{"52","ka",sizeof(PY_mb_ka),(unsigned char*)PY_mb_ka},
{"53","ke",sizeof(PY_mb_ke),(unsigned char*)PY_mb_ke},
{"58","ku",sizeof(PY_mb_ku),(unsigned char*)PY_mb_ku},
{"52","la",sizeof(PY_mb_la),(unsigned char*)PY_mb_la},
{"53","le",sizeof(PY_mb_le),(unsigned char*)PY_mb_le},
{"54","li",sizeof(PY_mb_li),(unsigned char*)PY_mb_li},
{"58","lu",sizeof(PY_mb_lu),(unsigned char*)PY_mb_lu},
{"58","lv",sizeof(PY_mb_lv),(unsigned char*)PY_mb_lv},
{"62","ma",sizeof(PY_mb_ma),(unsigned char*)PY_mb_ma},
{"63","me",sizeof(PY_mb_me),(unsigned char*)PY_mb_me},
{"64","mi",sizeof(PY_mb_mi),(unsigned char*)PY_mb_mi},
{"66","mo",sizeof(PY_mb_mo),(unsigned char*)PY_mb_mo},
{"68","mu",sizeof(PY_mb_mu),(unsigned char*)PY_mb_mu},
{"62","na",sizeof(PY_mb_na),(unsigned char*)PY_mb_na},
{"63","ne",sizeof(PY_mb_ne),(unsigned char*)PY_mb_ne},
{"64","ni",sizeof(PY_mb_ni),(unsigned char*)PY_mb_ni},
{"68","nu",sizeof(PY_mb_nu),(unsigned char*)PY_mb_nu},
{"68","nv",sizeof(PY_mb_nv),(unsigned char*)PY_mb_nv},
{"68","ou",sizeof(PY_mb_ou),(unsigned char*)PY_mb_ou},
{"72","pa",sizeof(PY_mb_pa),(unsigned char*)PY_mb_pa},
{"74","pi",sizeof(PY_mb_pi),(unsigned char*)PY_mb_pi},
{"76","po",sizeof(PY_mb_po),(unsigned char*)PY_mb_po},
{"78","pu",sizeof(PY_mb_pu),(unsigned char*)PY_mb_pu},
{"74","qi",sizeof(PY_mb_qi),(unsigned char*)PY_mb_qi},
{"78","qu",sizeof(PY_mb_qu),(unsigned char*)PY_mb_qu},
{"73","re",sizeof(PY_mb_re),(unsigned char*)PY_mb_re},
{"74","ri",sizeof(PY_mb_ri),(unsigned char*)PY_mb_ri},
{"78","ru",sizeof(PY_mb_ru),(unsigned char*)PY_mb_ru},
{"72","sa",sizeof(PY_mb_sa),(unsigned char*)PY_mb_sa},
{"73","se",sizeof(PY_mb_se),(unsigned char*)PY_mb_se},
{"74","si",sizeof(PY_mb_si),(unsigned char*)PY_mb_si},
{"78","su",sizeof(PY_mb_su),(unsigned char*)PY_mb_su},
{"82","ta",sizeof(PY_mb_ta),(unsigned char*)PY_mb_ta},
{"83","te",sizeof(PY_mb_te),(unsigned char*)PY_mb_te},
{"84","ti",sizeof(PY_mb_ti),(unsigned char*)PY_mb_ti},
{"88","tu",sizeof(PY_mb_tu),(unsigned char*)PY_mb_tu},
{"92","wa",sizeof(PY_mb_wa),(unsigned char*)PY_mb_wa},
{"96","wo",sizeof(PY_mb_wo),(unsigned char*)PY_mb_wo},
{"98","wu",sizeof(PY_mb_wu),(unsigned char*)PY_mb_wu},
{"94","xi",sizeof(PY_mb_xi),(unsigned char*)PY_mb_xi},
{"98","xu",sizeof(PY_mb_xu),(unsigned char*)PY_mb_xu},
{"92","ya",sizeof(PY_mb_ya),(unsigned char*)PY_mb_ya},
{"93","ye",sizeof(PY_mb_ye),(unsigned char*)PY_mb_ye},
{"94","yi",sizeof(PY_mb_yi),(unsigned char*)PY_mb_yi},
{"96","yo",sizeof(PY_mb_yo),(unsigned char*)PY_mb_yo},
{"98","yu",sizeof(PY_mb_yu),(unsigned char*)PY_mb_yu},
{"92","za",sizeof(PY_mb_za),(unsigned char*)PY_mb_za},
{"93","ze",sizeof(PY_mb_ze),(unsigned char*)PY_mb_ze},
{"94","zi",sizeof(PY_mb_zi),(unsigned char*)PY_mb_zi},
{"98","zu",sizeof(PY_mb_zu),(unsigned char*)PY_mb_zu},
{"264","ang",sizeof(PY_mb_ang),(unsigned char*)PY_mb_ang},
{"224","bai",sizeof(PY_mb_bai),(unsigned char*)PY_mb_bai},
{"226","ban",sizeof(PY_mb_ban),(unsigned char*)PY_mb_ban},
{"226","bao",sizeof(PY_mb_bao),(unsigned char*)PY_mb_bao},
{"234","bei",sizeof(PY_mb_bei),(unsigned char*)PY_mb_bei},
{"236","ben",sizeof(PY_mb_ben),(unsigned char*)PY_mb_ben},
{"243","bie",sizeof(PY_mb_bie),(unsigned char*)PY_mb_bie},
{"246","bin",sizeof(PY_mb_bin),(unsigned char*)PY_mb_bin},
{"224","cai",sizeof(PY_mb_cai),(unsigned char*)PY_mb_cai},
{"226","can",sizeof(PY_mb_can),(unsigned char*)PY_mb_can},
{"226","cao",sizeof(PY_mb_cao),(unsigned char*)PY_mb_cao},
{"242","cha",sizeof(PY_mb_cha),(unsigned char*)PY_mb_cha},
{"243","che",sizeof(PY_mb_che),(unsigned char*)PY_mb_che},
{"244","chi",sizeof(PY_mb_chi),(unsigned char*)PY_mb_chi},
{"248","chu",sizeof(PY_mb_chu),(unsigned char*)PY_mb_chu},
{"268","cou",sizeof(PY_mb_cou),(unsigned char*)PY_mb_cou},
{"284","cui",sizeof(PY_mb_cui),(unsigned char*)PY_mb_cui},
{"286","cun",sizeof(PY_mb_cun),(unsigned char*)PY_mb_cun},
{"286","cuo",sizeof(PY_mb_cuo),(unsigned char*)PY_mb_cuo},
{"324","dai",sizeof(PY_mb_dai),(unsigned char*)PY_mb_dai},
{"326","dan",sizeof(PY_mb_dan),(unsigned char*)PY_mb_dan},
{"326","dao",sizeof(PY_mb_dao),(unsigned char*)PY_mb_dao},
{"334","dei",sizeof(PY_mb_dei),(unsigned char*)PY_mb_dei},
{"343","die",sizeof(PY_mb_die),(unsigned char*)PY_mb_die},
{"348","diu",sizeof(PY_mb_diu),(unsigned char*)PY_mb_diu},
{"368","dou",sizeof(PY_mb_dou),(unsigned char*)PY_mb_dou},
{"384","dui",sizeof(PY_mb_dui),(unsigned char*)PY_mb_dui},
{"386","dun",sizeof(PY_mb_dun),(unsigned char*)PY_mb_dun},
{"386","duo",sizeof(PY_mb_duo),(unsigned char*)PY_mb_duo},
{"326","fan",sizeof(PY_mb_fan),(unsigned char*)PY_mb_fan},
{"334","fei",sizeof(PY_mb_fei),(unsigned char*)PY_mb_fei},
{"336","fen",sizeof(PY_mb_fen),(unsigned char*)PY_mb_fen},
{"368","fou",sizeof(PY_mb_fou),(unsigned char*)PY_mb_fou},
{"424","gai",sizeof(PY_mb_gai),(unsigned char*)PY_mb_gai},
{"426","gan",sizeof(PY_mb_gan),(unsigned char*)PY_mb_gan},
{"426","gao",sizeof(PY_mb_gao),(unsigned char*)PY_mb_gao},
{"434","gei",sizeof(PY_mb_gei),(unsigned char*)PY_mb_gei},
{"436","gen",sizeof(PY_mb_gen),(unsigned char*)PY_mb_gen},
{"468","gou",sizeof(PY_mb_gou),(unsigned char*)PY_mb_gou},
{"482","gua",sizeof(PY_mb_gua),(unsigned char*)PY_mb_gua},
{"484","gui",sizeof(PY_mb_gui),(unsigned char*)PY_mb_gui},
{"486","gun",sizeof(PY_mb_gun),(unsigned char*)PY_mb_gun},
{"486","guo",sizeof(PY_mb_guo),(unsigned char*)PY_mb_guo},
{"424","hai",sizeof(PY_mb_hai),(unsigned char*)PY_mb_hai},
{"426","han",sizeof(PY_mb_han),(unsigned char*)PY_mb_han},
{"426","hao",sizeof(PY_mb_hao),(unsigned char*)PY_mb_hao},
{"434","hei",sizeof(PY_mb_hei),(unsigned char*)PY_mb_hei},
{"436","hen",sizeof(PY_mb_hen),(unsigned char*)PY_mb_hen},
{"468","hou",sizeof(PY_mb_hou),(unsigned char*)PY_mb_hou},
{"482","hua",sizeof(PY_mb_hua),(unsigned char*)PY_mb_hua},
{"484","hui",sizeof(PY_mb_hui),(unsigned char*)PY_mb_hui},
{"486","hun",sizeof(PY_mb_hun),(unsigned char*)PY_mb_hun},
{"486","huo",sizeof(PY_mb_huo),(unsigned char*)PY_mb_huo},
{"542","jia",sizeof(PY_mb_jia),(unsigned char*)PY_mb_jia},
{"543","jie",sizeof(PY_mb_jie),(unsigned char*)PY_mb_jie},
{"546","jin",sizeof(PY_mb_jin),(unsigned char*)PY_mb_jin},
{"548","jiu",sizeof(PY_mb_jiu),(unsigned char*)PY_mb_jiu},
{"583","jue",sizeof(PY_mb_jue),(unsigned char*)PY_mb_jue},
{"586","jun",sizeof(PY_mb_jun),(unsigned char*)PY_mb_jun},
{"524","kai",sizeof(PY_mb_kai),(unsigned char*)PY_mb_kai},
{"526","kan",sizeof(PY_mb_kan),(unsigned char*)PY_mb_kan},
{"526","kao",sizeof(PY_mb_kao),(unsigned char*)PY_mb_kao},
{"536","ken",sizeof(PY_mb_ken),(unsigned char*)PY_mb_ken},
{"568","kou",sizeof(PY_mb_kou),(unsigned char*)PY_mb_kou},
{"582","kua",sizeof(PY_mb_kua),(unsigned char*)PY_mb_kua},
{"584","kui",sizeof(PY_mb_kui),(unsigned char*)PY_mb_kui},
{"586","kun",sizeof(PY_mb_kun),(unsigned char*)PY_mb_kun},
{"586","kuo",sizeof(PY_mb_kuo),(unsigned char*)PY_mb_kuo},
{"524","lai",sizeof(PY_mb_lai),(unsigned char*)PY_mb_lai},
{"526","lan",sizeof(PY_mb_lan),(unsigned char*)PY_mb_lan},
{"526","lao",sizeof(PY_mb_lao),(unsigned char*)PY_mb_lao},
{"534","lei",sizeof(PY_mb_lei),(unsigned char*)PY_mb_lei},
{"543","lie",sizeof(PY_mb_lie),(unsigned char*)PY_mb_lie},
{"546","lin",sizeof(PY_mb_lin),(unsigned char*)PY_mb_lin},
{"548","liu",sizeof(PY_mb_liu),(unsigned char*)PY_mb_liu},
{"568","lou",sizeof(PY_mb_lou),(unsigned char*)PY_mb_lou},
{"583","lue",sizeof(PY_mb_lue),(unsigned char*)PY_mb_lue},
{"586","lun",sizeof(PY_mb_lun),(unsigned char*)PY_mb_lun},
{"586","luo",sizeof(PY_mb_luo),(unsigned char*)PY_mb_luo},
{"624","mai",sizeof(PY_mb_mai),(unsigned char*)PY_mb_mai},
{"626","man",sizeof(PY_mb_man),(unsigned char*)PY_mb_man},
{"626","mao",sizeof(PY_mb_mao),(unsigned char*)PY_mb_mao},
{"634","mei",sizeof(PY_mb_mei),(unsigned char*)PY_mb_mei},
{"636","men",sizeof(PY_mb_men),(unsigned char*)PY_mb_men},
{"643","mie",sizeof(PY_mb_mie),(unsigned char*)PY_mb_mie},
{"646","min",sizeof(PY_mb_min),(unsigned char*)PY_mb_min},
{"648","miu",sizeof(PY_mb_miu),(unsigned char*)PY_mb_miu},
{"668","mou",sizeof(PY_mb_mou),(unsigned char*)PY_mb_mou},
{"624","nai",sizeof(PY_mb_nai),(unsigned char*)PY_mb_nai},
{"626","nan",sizeof(PY_mb_nan),(unsigned char*)PY_mb_nan},
{"626","nao",sizeof(PY_mb_nao),(unsigned char*)PY_mb_nao},
{"634","nei",sizeof(PY_mb_nei),(unsigned char*)PY_mb_nei},
{"636","nen",sizeof(PY_mb_nen),(unsigned char*)PY_mb_nen},
{"643","nie",sizeof(PY_mb_nie),(unsigned char*)PY_mb_nie},
{"646","nin",sizeof(PY_mb_nin),(unsigned char*)PY_mb_nin},
{"648","niu",sizeof(PY_mb_niu),(unsigned char*)PY_mb_niu},
{"683","nue",sizeof(PY_mb_nue),(unsigned char*)PY_mb_nue},
{"686","nuo",sizeof(PY_mb_nuo),(unsigned char*)PY_mb_nuo},
{"724","pai",sizeof(PY_mb_pai),(unsigned char*)PY_mb_pai},
{"726","pan",sizeof(PY_mb_pan),(unsigned char*)PY_mb_pan},
{"726","pao",sizeof(PY_mb_pao),(unsigned char*)PY_mb_pao},
{"734","pei",sizeof(PY_mb_pei),(unsigned char*)PY_mb_pei},
{"736","pen",sizeof(PY_mb_pen),(unsigned char*)PY_mb_pen},
{"743","pie",sizeof(PY_mb_pie),(unsigned char*)PY_mb_pie},
{"746","pin",sizeof(PY_mb_pin),(unsigned char*)PY_mb_pin},
{"768","pou",sizeof(PY_mb_pou),(unsigned char*)PY_mb_pou},
{"742","qia",sizeof(PY_mb_qia),(unsigned char*)PY_mb_qia},
{"743","qie",sizeof(PY_mb_qie),(unsigned char*)PY_mb_qie},
{"746","qin",sizeof(PY_mb_qin),(unsigned char*)PY_mb_qin},
{"748","qiu",sizeof(PY_mb_qiu),(unsigned char*)PY_mb_qiu},
{"783","que",sizeof(PY_mb_que),(unsigned char*)PY_mb_que},
{"786","qun",sizeof(PY_mb_qun),(unsigned char*)PY_mb_qun},
{"726","ran",sizeof(PY_mb_ran),(unsigned char*)PY_mb_ran},
{"726","rao",sizeof(PY_mb_rao),(unsigned char*)PY_mb_rao},
{"736","ren",sizeof(PY_mb_ren),(unsigned char*)PY_mb_ren},
{"768","rou",sizeof(PY_mb_rou),(unsigned char*)PY_mb_rou},
{"784","rui",sizeof(PY_mb_rui),(unsigned char*)PY_mb_rui},
{"786","run",sizeof(PY_mb_run),(unsigned char*)PY_mb_run},
{"786","ruo",sizeof(PY_mb_ruo),(unsigned char*)PY_mb_ruo},
{"724","sai",sizeof(PY_mb_sai),(unsigned char*)PY_mb_sai},
{"726","sao",sizeof(PY_mb_sao),(unsigned char*)PY_mb_sao},
{"726","san",sizeof(PY_mb_san),(unsigned char*)PY_mb_san},
{"736","sen",sizeof(PY_mb_sen),(unsigned char*)PY_mb_sen},
{"742","sha",sizeof(PY_mb_sha),(unsigned char*)PY_mb_sha},
{"743","she",sizeof(PY_mb_she),(unsigned char*)PY_mb_she},
{"744","shi",sizeof(PY_mb_shi),(unsigned char*)PY_mb_shi},
{"748","shu",sizeof(PY_mb_shu),(unsigned char*)PY_mb_shu},
{"768","sou",sizeof(PY_mb_sou),(unsigned char*)PY_mb_sou},
{"784","sui",sizeof(PY_mb_sui),(unsigned char*)PY_mb_sui},
{"786","sun",sizeof(PY_mb_sun),(unsigned char*)PY_mb_sun},
{"786","suo",sizeof(PY_mb_suo),(unsigned char*)PY_mb_suo},
{"824","tai",sizeof(PY_mb_tai),(unsigned char*)PY_mb_tai},
{"826","tan",sizeof(PY_mb_tan),(unsigned char*)PY_mb_tan},
{"826","tao",sizeof(PY_mb_tao),(unsigned char*)PY_mb_tao},
{"843","tie",sizeof(PY_mb_tie),(unsigned char*)PY_mb_tie},
{"868","tou",sizeof(PY_mb_tou),(unsigned char*)PY_mb_tou},
{"884","tui",sizeof(PY_mb_tui),(unsigned char*)PY_mb_tui},
{"886","tun",sizeof(PY_mb_tun),(unsigned char*)PY_mb_tun},
{"886","tuo",sizeof(PY_mb_tuo),(unsigned char*)PY_mb_tuo},
{"924","wai",sizeof(PY_mb_wai),(unsigned char*)PY_mb_wai},
{"926","wan",sizeof(PY_mb_wan),(unsigned char*)PY_mb_wan},
{"934","wei",sizeof(PY_mb_wei),(unsigned char*)PY_mb_wei},
{"936","wen",sizeof(PY_mb_wen),(unsigned char*)PY_mb_wen},
{"942","xia",sizeof(PY_mb_xia),(unsigned char*)PY_mb_xia},
{"943","xie",sizeof(PY_mb_xie),(unsigned char*)PY_mb_xie},
{"946","xin",sizeof(PY_mb_xin),(unsigned char*)PY_mb_xin},
{"948","xiu",sizeof(PY_mb_xiu),(unsigned char*)PY_mb_xiu},
{"983","xue",sizeof(PY_mb_xue),(unsigned char*)PY_mb_xue},
{"986","xun",sizeof(PY_mb_xun),(unsigned char*)PY_mb_xun},
{"926","yan",sizeof(PY_mb_yan),(unsigned char*)PY_mb_yan},
{"926","yao",sizeof(PY_mb_yao),(unsigned char*)PY_mb_yao},
{"946","yin",sizeof(PY_mb_yin),(unsigned char*)PY_mb_yin},
{"968","you",sizeof(PY_mb_you),(unsigned char*)PY_mb_you},
{"983","yue",sizeof(PY_mb_yue),(unsigned char*)PY_mb_yue},
{"986","yun",sizeof(PY_mb_yun),(unsigned char*)PY_mb_yun},
{"924","zai",sizeof(PY_mb_zai),(unsigned char*)PY_mb_zai},
{"926","zan",sizeof(PY_mb_zan),(unsigned char*)PY_mb_zan},
{"926","zao",sizeof(PY_mb_zao),(unsigned char*)PY_mb_zao},
{"934","zei",sizeof(PY_mb_zei),(unsigned char*)PY_mb_zei},
{"936","zen",sizeof(PY_mb_zen),(unsigned char*)PY_mb_zen},
{"942","zha",sizeof(PY_mb_zha),(unsigned char*)PY_mb_zha},
{"943","zhe",sizeof(PY_mb_zhe),(unsigned char*)PY_mb_zhe},
{"944","zhi",sizeof(PY_mb_zhi),(unsigned char*)PY_mb_zhi},
{"948","zhu",sizeof(PY_mb_zhu),(unsigned char*)PY_mb_zhu},
{"968","zou",sizeof(PY_mb_zou),(unsigned char*)PY_mb_zou},
{"984","zui",sizeof(PY_mb_zui),(unsigned char*)PY_mb_zui},
{"986","zun",sizeof(PY_mb_zun),(unsigned char*)PY_mb_zun},
{"986","zuo",sizeof(PY_mb_zuo),(unsigned char*)PY_mb_zuo},
{"2264","bang",sizeof(PY_mb_bang),(unsigned char*)PY_mb_bang},
{"2364","beng",sizeof(PY_mb_beng),(unsigned char*)PY_mb_beng},
{"2426","bian",sizeof(PY_mb_bian),(unsigned char*)PY_mb_bian},
{"2426","biao",sizeof(PY_mb_biao),(unsigned char*)PY_mb_biao},
{"2464","bing",sizeof(PY_mb_bing),(unsigned char*)PY_mb_bing},
{"2264","cang",sizeof(PY_mb_cang),(unsigned char*)PY_mb_cang},
{"2364","ceng",sizeof(PY_mb_ceng),(unsigned char*)PY_mb_ceng},
{"2424","chai",sizeof(PY_mb_chai),(unsigned char*)PY_mb_chai},
{"2426","chan",sizeof(PY_mb_chan),(unsigned char*)PY_mb_chan},
{"2426","chao",sizeof(PY_mb_chao),(unsigned char*)PY_mb_chao},
{"2436","chen",sizeof(PY_mb_chen),(unsigned char*)PY_mb_chen},
{"2468","chou",sizeof(PY_mb_chou),(unsigned char*)PY_mb_chou},
{"2484","chuai",sizeof(PY_mb_chuai),(unsigned char*)PY_mb_chuai},
{"2484","chui",sizeof(PY_mb_chui),(unsigned char*)PY_mb_chui},
{"2486","chun",sizeof(PY_mb_chun),(unsigned char*)PY_mb_chun},
{"2486","chuo",sizeof(PY_mb_chuo),(unsigned char*)PY_mb_chuo},
{"2664","cong",sizeof(PY_mb_cong),(unsigned char*)PY_mb_cong},
{"2826","cuan",sizeof(PY_mb_cuan),(unsigned char*)PY_mb_cuan},
{"3264","dang",sizeof(PY_mb_dang),(unsigned char*)PY_mb_dang},
{"3364","deng",sizeof(PY_mb_deng),(unsigned char*)PY_mb_deng},
{"3426","dian",sizeof(PY_mb_dian),(unsigned char*)PY_mb_dian},
{"3426","diao",sizeof(PY_mb_diao),(unsigned char*)PY_mb_diao},
{"3464","ding",sizeof(PY_mb_ding),(unsigned char*)PY_mb_ding},
{"3664","dong",sizeof(PY_mb_dong),(unsigned char*)PY_mb_dong},
{"3826","duan",sizeof(PY_mb_duan),(unsigned char*)PY_mb_duan},
{"3264","fang",sizeof(PY_mb_fang),(unsigned char*)PY_mb_fang},
{"3364","feng",sizeof(PY_mb_feng),(unsigned char*)PY_mb_feng},
{"4264","gang",sizeof(PY_mb_gang),(unsigned char*)PY_mb_gang},
{"4364","geng",sizeof(PY_mb_geng),(unsigned char*)PY_mb_geng},
{"4664","gong",sizeof(PY_mb_gong),(unsigned char*)PY_mb_gong},
{"4824","guai",sizeof(PY_mb_guai),(unsigned char*)PY_mb_guai},
{"4826","guan",sizeof(PY_mb_guan),(unsigned char*)PY_mb_guan},
{"4264","hang",sizeof(PY_mb_hang),(unsigned char*)PY_mb_hang},
{"4364","heng",sizeof(PY_mb_heng),(unsigned char*)PY_mb_heng},
{"4664","hong",sizeof(PY_mb_hong),(unsigned char*)PY_mb_hong},
{"4824","huai",sizeof(PY_mb_huai),(unsigned char*)PY_mb_huai},
{"4826","huan",sizeof(PY_mb_huan),(unsigned char*)PY_mb_huan},
{"5426","jian",sizeof(PY_mb_jian),(unsigned char*)PY_mb_jian},
{"5426","jiao",sizeof(PY_mb_jiao),(unsigned char*)PY_mb_jiao},
{"5464","jing",sizeof(PY_mb_jing),(unsigned char*)PY_mb_jing},
{"5826","juan",sizeof(PY_mb_juan),(unsigned char*)PY_mb_juan},
{"5264","kang",sizeof(PY_mb_kang),(unsigned char*)PY_mb_kang},
{"5364","keng",sizeof(PY_mb_keng),(unsigned char*)PY_mb_keng},
{"5664","kong",sizeof(PY_mb_kong),(unsigned char*)PY_mb_kong},
{"5824","kuai",sizeof(PY_mb_kuai),(unsigned char*)PY_mb_kuai},
{"5826","kuan",sizeof(PY_mb_kuan),(unsigned char*)PY_mb_kuan},
{"5264","lang",sizeof(PY_mb_lang),(unsigned char*)PY_mb_lang},
{"5366","leng",sizeof(PY_mb_leng),(unsigned char*)PY_mb_leng},
{"5426","lian",sizeof(PY_mb_lian),(unsigned char*)PY_mb_lian},
{"5426","liao",sizeof(PY_mb_liao),(unsigned char*)PY_mb_liao},
{"5464","ling",sizeof(PY_mb_ling),(unsigned char*)PY_mb_ling},
{"5664","long",sizeof(PY_mb_long),(unsigned char*)PY_mb_long},
{"5826","luan",sizeof(PY_mb_luan),(unsigned char*)PY_mb_luan},
{"6264","mang",sizeof(PY_mb_mang),(unsigned char*)PY_mb_mang},
{"6364","meng",sizeof(PY_mb_meng),(unsigned char*)PY_mb_meng},
{"6426","mian",sizeof(PY_mb_mian),(unsigned char*)PY_mb_mian},
{"6426","miao",sizeof(PY_mb_miao),(unsigned char*)PY_mb_miao},
{"6464","ming",sizeof(PY_mb_ming),(unsigned char*)PY_mb_ming},
{"6264","nang",sizeof(PY_mb_nang),(unsigned char*)PY_mb_nang},
{"6364","neng",sizeof(PY_mb_neng),(unsigned char*)PY_mb_neng},
{"6426","nian",sizeof(PY_mb_nian),(unsigned char*)PY_mb_nian},
{"6426","niao",sizeof(PY_mb_niao),(unsigned char*)PY_mb_niao},
{"6464","ning",sizeof(PY_mb_ning),(unsigned char*)PY_mb_ning},
{"6664","nong",sizeof(PY_mb_nong),(unsigned char*)PY_mb_nong},
{"6826","nuan",sizeof(PY_mb_nuan),(unsigned char*)PY_mb_nuan},
{"7264","pang",sizeof(PY_mb_pang),(unsigned char*)PY_mb_pang},
{"7364","peng",sizeof(PY_mb_peng),(unsigned char*)PY_mb_peng},
{"7426","pian",sizeof(PY_mb_pian),(unsigned char*)PY_mb_pian},
{"7426","piao",sizeof(PY_mb_piao),(unsigned char*)PY_mb_piao},
{"7464","ping",sizeof(PY_mb_ping),(unsigned char*)PY_mb_ping},
{"7426","qian",sizeof(PY_mb_qian),(unsigned char*)PY_mb_qian},
{"7426","qiao",sizeof(PY_mb_qiao),(unsigned char*)PY_mb_qiao},
{"7464","qing",sizeof(PY_mb_qing),(unsigned char*)PY_mb_qing},
{"7826","quan",sizeof(PY_mb_quan),(unsigned char*)PY_mb_quan},
{"7264","rang",sizeof(PY_mb_rang),(unsigned char*)PY_mb_rang},
{"7364","reng",sizeof(PY_mb_reng),(unsigned char*)PY_mb_reng},
{"7664","rong",sizeof(PY_mb_rong),(unsigned char*)PY_mb_rong},
{"7826","ruan",sizeof(PY_mb_ruan),(unsigned char*)PY_mb_ruan},
{"7264","sang",sizeof(PY_mb_sang),(unsigned char*)PY_mb_sang},
{"7364","seng",sizeof(PY_mb_seng),(unsigned char*)PY_mb_seng},
{"7424","shai",sizeof(PY_mb_shai),(unsigned char*)PY_mb_shai},
{"7426","shan",sizeof(PY_mb_shan),(unsigned char*)PY_mb_shan},
{"7426","shao",sizeof(PY_mb_shao),(unsigned char*)PY_mb_shao},
{"7434","shei",sizeof(PY_mb_shei),(unsigned char*)PY_mb_shei},
{"7436","shen",sizeof(PY_mb_shen),(unsigned char*)PY_mb_shen},
{"7468","shou",sizeof(PY_mb_shou),(unsigned char*)PY_mb_shou},
{"7482","shua",sizeof(PY_mb_shua),(unsigned char*)PY_mb_shua},
{"7484","shui",sizeof(PY_mb_shui),(unsigned char*)PY_mb_shui},
{"7486","shun",sizeof(PY_mb_shun),(unsigned char*)PY_mb_shun},
{"7486","shuo",sizeof(PY_mb_shuo),(unsigned char*)PY_mb_shuo},
{"7664","song",sizeof(PY_mb_song),(unsigned char*)PY_mb_song},
{"7826","suan",sizeof(PY_mb_suan),(unsigned char*)PY_mb_suan},
{"8264","tang",sizeof(PY_mb_tang),(unsigned char*)PY_mb_tang},
{"8364","teng",sizeof(PY_mb_teng),(unsigned char*)PY_mb_teng},
{"8426","tian",sizeof(PY_mb_tian),(unsigned char*)PY_mb_tian},
{"8426","tiao",sizeof(PY_mb_tiao),(unsigned char*)PY_mb_tiao},
{"8464","ting",sizeof(PY_mb_ting),(unsigned char*)PY_mb_ting},
{"8664","tong",sizeof(PY_mb_tong),(unsigned char*)PY_mb_tong},
{"8826","tuan",sizeof(PY_mb_tuan),(unsigned char*)PY_mb_tuan},
{"9264","wang",sizeof(PY_mb_wang),(unsigned char*)PY_mb_wang},
{"9364","weng",sizeof(PY_mb_weng),(unsigned char*)PY_mb_weng},
{"9426","xian",sizeof(PY_mb_xian),(unsigned char*)PY_mb_xian},
{"9426","xiao",sizeof(PY_mb_xiao),(unsigned char*)PY_mb_xiao},
{"9464","xing",sizeof(PY_mb_xing),(unsigned char*)PY_mb_xing},
{"9826","xuan",sizeof(PY_mb_xuan),(unsigned char*)PY_mb_xuan},
{"9264","yang",sizeof(PY_mb_yang),(unsigned char*)PY_mb_yang},
{"9464","ying",sizeof(PY_mb_ying),(unsigned char*)PY_mb_ying},
{"9664","yong",sizeof(PY_mb_yong),(unsigned char*)PY_mb_yong},
{"9826","yuan",sizeof(PY_mb_yuan),(unsigned char*)PY_mb_yuan},
{"9264","zang",sizeof(PY_mb_zang),(unsigned char*)PY_mb_zang},
{"9364","zeng",sizeof(PY_mb_zeng),(unsigned char*)PY_mb_zeng},
{"9424","zhai",sizeof(PY_mb_zhai),(unsigned char*)PY_mb_zhai},
{"9426","zhan",sizeof(PY_mb_zhan),(unsigned char*)PY_mb_zhan},
{"9426","zhao",sizeof(PY_mb_zhao),(unsigned char*)PY_mb_zhao},
{"9436","zhen",sizeof(PY_mb_zhen),(unsigned char*)PY_mb_zhen},
{"9468","zhou",sizeof(PY_mb_zhou),(unsigned char*)PY_mb_zhou},
{"9482","zhua",sizeof(PY_mb_zhua),(unsigned char*)PY_mb_zhua},
{"9484","zhui",sizeof(PY_mb_zhui),(unsigned char*)PY_mb_zhui},
{"9486","zhun",sizeof(PY_mb_zhun),(unsigned char*)PY_mb_zhun},
{"9486","zhuo",sizeof(PY_mb_zhuo),(unsigned char*)PY_mb_zhuo},
{"9664","zong",sizeof(PY_mb_zong),(unsigned char*)PY_mb_zong},
{"9826","zuan",sizeof(PY_mb_zuan),(unsigned char*)PY_mb_zuan},
{"24264","chang",sizeof(PY_mb_chang),(unsigned char*)PY_mb_chang},
{"24364","cheng",sizeof(PY_mb_cheng),(unsigned char*)PY_mb_cheng},
{"24664","chong",sizeof(PY_mb_chong),(unsigned char*)PY_mb_chong},
{"24826","chuan",sizeof(PY_mb_chuan),(unsigned char*)PY_mb_chuan},
{"48264","guang",sizeof(PY_mb_guang),(unsigned char*)PY_mb_guang},
{"48264","huang",sizeof(PY_mb_huang),(unsigned char*)PY_mb_huang},
{"54264","jiang",sizeof(PY_mb_jiang),(unsigned char*)PY_mb_jiang},
{"54664","jiong",sizeof(PY_mb_jiong),(unsigned char*)PY_mb_jiong},
{"58264","kuang",sizeof(PY_mb_kuang),(unsigned char*)PY_mb_kuang},
{"54264","liang",sizeof(PY_mb_liang),(unsigned char*)PY_mb_liang},
{"64264","niang",sizeof(PY_mb_niang),(unsigned char*)PY_mb_niang},
{"74264","qiang",sizeof(PY_mb_qiang),(unsigned char*)PY_mb_qiang},
{"74664","qiong",sizeof(PY_mb_qiong),(unsigned char*)PY_mb_qiong},
{"74264","shang",sizeof(PY_mb_shang),(unsigned char*)PY_mb_shang},
{"74364","sheng",sizeof(PY_mb_sheng),(unsigned char*)PY_mb_sheng},
{"74824","shuai",sizeof(PY_mb_shuai),(unsigned char*)PY_mb_shuai},
{"74826","shuan",sizeof(PY_mb_shuan),(unsigned char*)PY_mb_shuan},
{"94264","xiang",sizeof(PY_mb_xiang),(unsigned char*)PY_mb_xiang},
{"94664","xiong",sizeof(PY_mb_xiong),(unsigned char*)PY_mb_xiong},
{"94264","zhang",sizeof(PY_mb_zhang),(unsigned char*)PY_mb_zhang},
{"94364","zheng",sizeof(PY_mb_zheng),(unsigned char*)PY_mb_zheng},
{"94664","zhong",sizeof(PY_mb_zhong),(unsigned char*)PY_mb_zhong},
{"94824","zhuai",sizeof(PY_mb_zhuai),(unsigned char*)PY_mb_zhuai},
{"94826","zhuan",sizeof(PY_mb_zhuan),(unsigned char*)PY_mb_zhuan},
{"248264","chuang",sizeof(PY_mb_chuang), (unsigned char*)PY_mb_chuang},
{"748264","shuang",sizeof(PY_mb_shuang), (unsigned char*)PY_mb_shuang},
{"948264","zhuang",sizeof(PY_mb_zhuang), (unsigned char*)PY_mb_zhuang},
};

int size_of_pyindex()
{
	return sizeof(py_index3)/sizeof(py_index3[0]);
}
