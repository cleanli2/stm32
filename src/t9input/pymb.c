#include "pyinput.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞò²Î¿¼×ÔÍøÂç²¢¼ÓÒÔĞŞ¸Ä
//ALIENTEKÕ½½¢STM32¿ª·¢°å
//Æ´ÒôÊäÈë·¨ Çı¶¯´úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//ĞŞ¸ÄÈÕÆÚ:2012/9/22
//°æ±¾£ºV1.0			    
//¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾  	 												    								  
//////////////////////////////////////////////////////////////////////////////////

//ºº×ÖÅÅÁĞ±í
const unsigned char PY_mb_space []={"£¬¡££»¡ı¡¢£¡£¿¡¶¡·¡¡£º£¨£©¡°¡±"};
const unsigned char PY_mb_a     []={"°¡°¢ëçß¹ï¹…àÄåHºÇëç"};
const unsigned char PY_mb_ai    []={"°®°£°¤°¥°¦°§°¨°©°ª°«°¬°­°¯ŞßàÈàÉæÈè¨êÓíÁïÍö°"};
const unsigned char PY_mb_an    []={"°²°³°´°µ°¶°¸°°°±ÚÏ°·ÛûŞîáíâÖèñï§ğÆ÷ö"};
const unsigned char PY_mb_ang   []={"°º°»°¹óa"};
const unsigned char PY_mb_ao    []={"°¼°½°¾°¿°À°Á°Â°Ã°ÄÛêŞÖà»òüöË÷¡÷éá®âÚåÛæÁæñéáñú"};
const unsigned char PY_mb_ba    []={"°Ë°Í°Ö°Î°Å°Æ°Ç°È°É°Ê°Ì°Ï°Ğ°Ñ°Ò°Ó°Ô°ÕÜØİÃá±å±îÙôÎöÑ÷É"};
const unsigned char PY_mb_bai   []={"°×°Ø°Ù°Ú°Û°Ü°İ°Ş²®Şãêş"};
const unsigned char PY_mb_ban   []={"°ë°ì°ß°à°á°â°ã°ä°å°æ°ç°è°é°ê°íÚæÛàêÚîÓñ£ñ­ô²"};
const unsigned char PY_mb_bang  []={"°î°ï°ğ°ñ°ò°ó°ô°õ°ö°÷°ø°ùİòäº"};
const unsigned char PY_mb_bao   []={"°ü±¦±£±§±¨±©°ú°û°ı°ş±¡±¢±¤±¥±ª±«±¬ÅÙÅÚÙèİáæßìÒğ±ñÙõÀöµ"};
const unsigned char PY_mb_bei   []={"±³±´±±±¶±­±®±¯±°±²±µ±·±¸±¹±º±»±ÛØÃÚéÚıÛıİÉİíßÂã£íÕğÇñØöÍ÷¹"};;
const unsigned char PY_mb_ben   []={"±¾±¿±¼±½ÛÎÛĞêÚï¼"};
const unsigned char PY_mb_beng  []={"°ö±À±Á±Â±Ã±Ä±ÅÜ¡àÔê´"};
const unsigned char PY_mb_bi    []={"±Æ±Ç±È±É±Ê±Ë±Ì±Í±Î±Ï±Ğ±Ñ±Ò±Ó±Ô±ÕØ°±Ö±×±Ø±Ù±Ú±Û±Ü±İÃØÃÚÙÂÛıÜÅÜêİ©İÉŞµßÁßÙáùâØã¹ääå¨åöåşæ¾æÔèµêÚî¯îéïõñÔóÙóëó÷ô°ôÅõÏ÷Â"};
const unsigned char PY_mb_bian  []={"±ß±à±á±ã±ä±å±Ş±â±æ±ç±è±éØÒÛÍÜĞâíãêçÂìÔí¾íÜñ¹ñÛòùóÖöı"};
const unsigned char PY_mb_biao  []={"±í±ê±ë±ìæ»æôè¼ì©ì­ì®ïÚïğñ¦ñÑ÷§÷Ô"};
const unsigned char PY_mb_bie   []={"±ğ±î±ï±ñõ¿"};
const unsigned char PY_mb_bin   []={"±ò±ó±ô±õ±ö±÷ÙÏáÙçÍçãéÄéëë÷ïÙ÷Æ÷Ş"};
const unsigned char PY_mb_bing  []={"±ø±ù±ú±û±ü±ı±ş²¡²¢ÆÁÙ÷ÙûÚûŞğéÄ"};
const unsigned char PY_mb_bo    []={"°Ø°Ù°ş±¡²©²£²¤²·²¥²¦²§²¨²ª²«²¬²­²®²¯²°²±²²²³²´²µØÃÙñŞ¬à£âÄéŞë¢íçîàğ¾ô¤õÀõËõÛ"};
const unsigned char PY_mb_bu    []={"²»²¼²½±¤²¶²·²¸²¹²º²¾²¿²ÀÆÒß²åÍê³êÎîĞîßõ³"};
const unsigned char PY_mb_ca    []={"²Áàêíå"};
const unsigned char PY_mb_cai   []={"²Å²Æ²Ç²Â²Ã²Ä²È²É²Ê²Ë²Ì"};
const unsigned char PY_mb_can   []={"²Í²Î²Ï²Ğ²Ñ²Ò²Óåîæîè²ôÓ÷õ"};
const unsigned char PY_mb_cang  []={"²Ô²Õ²Ö²×²ØØ÷"};
const unsigned char PY_mb_cao   []={"²İ²Ù²Ú²Û²ÜÜ³àĞäîó©ô½"};
const unsigned char PY_mb_ce    []={"²à²á²â²Ş²ßâü"};
const unsigned char PY_mb_cen   []={"²Îá¯ä¹"};
const unsigned char PY_mb_ceng  []={"²ã²äÔøàá"};
const unsigned char PY_mb_cha   []={"²å²æ²ç²è²é²ê²ë²ì²í²î²ïÉ²ÔûñÃàêâªâÇãâæ±è¾é«é¶éßïÊïï"};
const unsigned char PY_mb_chai  []={"²î²ğ²ñ²òÙ­îÎğûò²ö·"};
const unsigned char PY_mb_chan  []={"²ú²û²ü²ó²ô²õ²ö²÷²ø²ùåîµ¥ÙæÚÆİÛâÜâãäıå¤åñæ¿êèìøïâó¸õğ"};
const unsigned char PY_mb_chang []={"³¤³§²ı²ş³¡³¢³£³¥³¦³¨³©³ª³«ØöÛËÜÉİÅáäâêã®ãÑæ½æÏêÆë©öğ"};
const unsigned char PY_mb_chao  []={"³¬³­³®³¯³°³±³²³³³´´Â½Ëâ÷êËñé"};
const unsigned char PY_mb_che   []={"³µ³¶³·³¸³¹³ºÛååøíº"};
const unsigned char PY_mb_chen  []={"³»³¼³½³¾³¿³À³Á³Â³Ã³Ä³ÆÚÈÚßŞÓàÁå·è¡é´í×ö³"};
const unsigned char PY_mb_cheng []={"³Å³Æ³Ç³È³É³Ê³Ë³Ì³Í³Î³Ï³Ğ³Ñ³Ò³ÓÊ¢Ø©ÛôàááçèÇèßëóîªîñîõñÎòÉõ¨"};
const unsigned char PY_mb_chi   []={"³Ô³Õ³Ö³×³Ø³Ù³Ú³Û³Ü³İ³Ş³ß³à³á³â³ãó×ß³ßêÙÑÛæÜ¯Üİà´àÍáÜâÁæÊë·í÷ğ·ñ¡ñİò¿ó¤óøôùõØ÷Î"};
const unsigned char PY_mb_chong []={"³ä³å³æ³ç³èÖÖÖØÜûâçã¿ï¥ô©ô¾öÅ"};
const unsigned char PY_mb_chou  []={"³é³ê³ó³ë³ì³í³î³ï³ğ³ñ³ò³ôÙ±àüã°ñ¬öÅ"};
const unsigned char PY_mb_chu   []={"³ö³õ³÷³ø³ù³ú³û³ü³ı³ş´¡´¢´£´¤´¥´¦ĞóØ¡Û»âğãÀç©èÆèúéËñÒòÜõé÷í"};
const unsigned char PY_mb_chuai []={"´§ŞõàÜëúõß"};
const unsigned char PY_mb_chuan []={"´¨´©´ª´«´¬´­´®â¶å×çİë°îËô­"};
const unsigned char PY_mb_chuang[]={"´¯´°´±´²´³´´âëğÚ"};
const unsigned char PY_mb_chui  []={"´µ´¶´·´¸´¹Úïé¢é³"};
const unsigned char PY_mb_chun  []={"´º´»´¼´½´¾´¿´Àİ»ğÈòí"};
const unsigned char PY_mb_chuo  []={"´Á´Âà¨åÁê¡õÖöº"};
const unsigned char PY_mb_ci    []={"´Ë´Ì´Í´ÎËÅ²î´Ã´Ä´Å´Æ´Ç´È´É´Ê×ÈÜëßÚìôğËôÒôÙ"};
const unsigned char PY_mb_cong  []={"´Ó´Ô´Ï´Ğ´Ñ´ÒÜÊäÈæõçıè®èÈ"};
const unsigned char PY_mb_cou   []={"´Õé¨ê£ëí"};
const unsigned char PY_mb_cu    []={"´Ö´×´Ø´ÙİıáŞâ§éãõ¡õ¾õí"};
const unsigned char PY_mb_cuan  []={"´Ú´Û´ÜÔÜÙàß¥ìàïé"};
const unsigned char PY_mb_cui   []={"´İ´Ş´ß´à´á´â´ã´äİÍßıã²è­éÁë¥ö¿"};
const unsigned char PY_mb_cun   []={"´å´æ´çââñå"};
const unsigned char PY_mb_cuo   []={"´è´é´ê´ë´ì´íØÈáÏëâï±ïóğîğûõºõãõò"};
const unsigned char PY_mb_da    []={"´ó´î´ï´ğ´ñ´òŞÇßÕàªâòæ§í³ğãñ×óÎ÷°÷²"};
const unsigned char PY_mb_dai   []={"´ó´ô´õ´ö´÷´ø´ù´ú´û´ü´ı´şµ¡÷ìÜ¤ß°ß¾á·åÊææçªçé"};
const unsigned char PY_mb_dan   []={"µ¢µ£µ¤µ¥µ¦µ§µ¨µ©µªµ«µ¬µ­µ®µ¯µ°ÉÄÊ¯ÙÙİÌà¢å£ééêæíñğãğ÷ñõóì"};
const unsigned char PY_mb_dang  []={"µ±µ²µ³µ´µµÚÔÛÊİĞå´í¸îõñÉ"};
const unsigned char PY_mb_dao   []={"µ¶µ·µ¸µ¹µºµ»µ¼µ½µ¾µ¿µÀµÁØÖß¶àüâáë®ìâôî"};
const unsigned char PY_mb_de    []={"µÂµÃµÄµØï½"};
const unsigned char PY_mb_dei   []={"µÃ"};
const unsigned char PY_mb_deng  []={"³ÎµÅµÆµÇµÈµÉµÊµËàâáØê­íãïëô£"};
const unsigned char PY_mb_di    []={"µÄµÖµ×µØµÙµÚµÛµÜµİµÌµÍµÎµÏµĞµÑµÒµÓµÔµÕµŞÌáØµÙáÚ®ÚĞÛ¡Ûæİ¶àÖæ·èÜé¦êëíÆíÚíûïáôÆ÷¾"};
const unsigned char PY_mb_dia   []={"àÇ"};
const unsigned char PY_mb_dian  []={"µçµêµëµãµäµåµìµíµîµßµàµáµâµæµèµéÚçÛãáÛçèîäñ°ñ²ô¡õÚ"};
const unsigned char PY_mb_diao  []={"µõµóµôµöµ÷µïµğµñµòîöï¢õõöô"};
const unsigned char PY_mb_die   []={"µøµùµúµûµüµıµşÛìÜ¦Şéà©ëºğ¬ñóõŞöø"};
const unsigned char PY_mb_ding  []={"¶¡¶¢¶£¶¤¶¥¶¦¶§¶¨¶©Øêà¤çàëëíÖî®îúğÛñôôú"};
const unsigned char PY_mb_diu   []={"¶ªîû"};
const unsigned char PY_mb_dong  []={"¶«¶¬¶­¶®¶¯¶°¶±¶²¶³¶´ßËá´á¼ë±ëËëØíÏğ´"};
const unsigned char PY_mb_dou   []={"¶¹¶º¶»¶¼¶µ¶¶¶·¶¸İúñ¼ò½óû"};
const unsigned char PY_mb_du    []={"¶Ç¶È¶É¶¼¶½¶¾¶¿¶À¶Á¶Â¶Ã¶Ä¶Å¶Æ¶ÊÜ¶à½äÂèüë¹ó¼óÆ÷Ç÷ò"};
const unsigned char PY_mb_duan  []={"¶Ë¶Ì¶Í¶Î¶Ï¶Ğé²ìÑóı"};
const unsigned char PY_mb_dui   []={"¶Ñ¶Ò¶Ó¶Ôí¡í­íÔ"};
const unsigned char PY_mb_dun   []={"¶Ü¶Õ¶Ö¶×¶Ø¶Ù¶Ú¶Û¶İãçìÀí»íâíïïæõ»"};
const unsigned char PY_mb_duo   []={"¶à¶ä¶È¶Ş¶ß¶á¶â¶ã¶å¶æ¶ç¶è¶éÍÔßÍßáãõç¶îìñÖõâ"};
const unsigned char PY_mb_e     []={"°¢¶ê¶ë¶ì¶í¶î¶ï¶ğ¶ñ¶ò¶ó¶ô¶õ¶öÅ¶Ø¬ÚÌÛÑÜÃİ­İàßÀãµåíæ¹éîëñï°ïÉğÊò¦öù"};
const unsigned char PY_mb_ei    []={"ÚÀ"};
const unsigned char PY_mb_en    []={"¶÷İìŞô"};
const unsigned char PY_mb_er    []={"¶ş¶ø¶ù¶ú¶û¶ü¶ı·¡åÇçíîïğ¹öÜ"};
const unsigned char PY_mb_fa    []={"·¢·£·¤·¥·¦·§·¨·©ÛÒíÀ"};
const unsigned char PY_mb_fan   []={"·²·³·´·µ·¶···¸·¹·º·ª·«·¬·­·®·¯·°·±Ş¬ŞÀá¦èóìÜî²õì"};
const unsigned char PY_mb_fang  []={"·»·¼·½·¾·¿·À·Á·Â·Ã·Ä·ÅØÎÚúèÊîÕô³öĞ"};
const unsigned char PY_mb_fei   []={"·Æ·Ç·È·É·Ê·Ë·Ì·Í·Î·Ï·Ğ·ÑÜÀáôì³ã­åúç³äÇé¼êÚëèìéíÉïĞğòòãóõôäö­öî"};
const unsigned char PY_mb_fen   []={"·Ö·Ò·Ü·İ·Ş·ß·à·Ó·Ô·Õ·×·Ø·Ù·Ú·ÛÙÇå¯çãèûêÚö÷÷÷"};
const unsigned char PY_mb_feng  []={"·á·â·ã·ä·å·æ·ç·ï·è·é·ê·ë·ì·í·îÙºÛºİ×ßôããí¿"};
const unsigned char PY_mb_fo    []={"·ğ"};
const unsigned char PY_mb_fou   []={"·ñó¾"};
const unsigned char PY_mb_fu    []={"¸´·ğ¸µ¸¶¸·¸¸·ò·ó·ô·õ¸¾·ö·÷·ø·ù·ú·û·ü·ı·ş¸¡¸¢¸£¸¤¸¥¸¦¸§¸¨¸©¸ª¸«¸¬¸­¸®¸¯¸°¸±¸²¸³¸¹¸º¸»¸¼¸½¸¿¸ÀÙëÙìÚâÛ®Ü½ÜÀÜŞÜòİ³İÊŞÔß»á¥âöäæåõæÚæâç¦ç¨èõêçìğíÉíêíëî·ïûğ¥ò¶òİòğòóôïõÃõÆöÖöû"};
const unsigned char PY_mb_ga    []={"¸Á¸Â¼Ğ¿§Ù¤ŞÎæØæÙê¸îÅ"};
const unsigned char PY_mb_gai   []={"¸Ã¸Ä¸Å¸Æ¸Ç¸È½æØ¤ÚëÛòê®êàëÜ"};
const unsigned char PY_mb_gan   []={"¸É¸Ê¸Ë¸Ì¸Í¸Î¸Ï¸Ğ¸Ñ¸Ò¸ÓÛáÜÕŞÏß¦ãïäÆä÷ç¤éÏêºí·ğáôû"};
const unsigned char PY_mb_gang  []={"¸Ô¸Õ¸Ö¸×¸Ø¸Ù¸Ú¸Û¸Ü¿¸í°î¸óà"};
const unsigned char PY_mb_gao   []={"¸æ¸Ş¸ß¸à¸İ¸á¸â¸ã¸ä¸åØºÚ¾Û¬Ş»çÉéÀéÂê½ï¯"};
const unsigned char PY_mb_ge    []={"¸ö¸÷¸Ç¸ç÷Àëõ¸è¸é¸ê¸ë¸ì¸í¸î¸ï¸ğ¸ñ¸ò¸ó¸ô¸õºÏ¿©ØªØîÛÁÛÙÜªàÃæüë¡îşïÓñËò´ô´"};
const unsigned char PY_mb_gei   []={"¸ø"};
const unsigned char PY_mb_gen   []={"¸ù¸úØ¨İ¢ßçôŞ"};
const unsigned char PY_mb_geng  []={"¸û¸ü¸ı¸ş¹¡¹¢¹£¾±ßìâÙç®öá"};
const unsigned char PY_mb_gong  []={"¹¤¹²¹¥¹¦¹§¹«¹¬¹­¹¨¹©¹ª¹®¹¯¹°¹±ò¼ŞÃçîëÅö¡"};
const unsigned char PY_mb_gou   []={"¹³¹´¹µ¹¶¹»¹¸¹¹¹ºØşÚ¸¹·óôá¸åÜæÅçÃèÛêíì°óÑ÷¸"};
const unsigned char PY_mb_gu    []={"¹Ã¹À¹Á¹Â¹½¹¾¹¼¹¿¹Ä¹Å¹Æ¹Ç¹È¹É¹Ê¹Ë¹Ì¹Í¼ÖØÅÚ¬İÔáÄãéèôéïêôêöëûì±î­î¹îÜïÀğ³ğÀğóòÁôşõıöñ÷½"};
const unsigned char PY_mb_gua   []={"¹Î¹Ï¹Ğ¹Ñ¹Ò¹ÓØÔÚ´ßÉèéëÒğ»"};
const unsigned char PY_mb_guai  []={"¹Ô¹Õ¹ÖŞâ"};
const unsigned char PY_mb_guan  []={"¹Ø¹Ù¹Ú¹Û¹Ü¹İ¹Ş¹ß¹à¹áÂÚÙÄİ¸¹×ŞèäÊîÂğÙ÷¤"};
const unsigned char PY_mb_guang []={"¹â¹ã¹äßÛáîèæë×"};
const unsigned char PY_mb_gui   []={"¹å¹æ¹ç¹è¹é¹ê¹ë¹ì¹í¹î¹ğ¹ñ¹ò¹ó¹ô¹ïÈ²ØĞØÛâÑå³æ£èíêÁêĞğ§óşöÙ÷¬"};
const unsigned char PY_mb_gun   []={"¹õ¹ö¹÷ÙòçµíŞöç"};
const unsigned char PY_mb_guo   []={"¹ø¹ù¹ú¹û¹ü¹ıÎĞÙåÛöŞâßÃàşáÆâ£é¤ë½ï¾ñøòäòå"};
const unsigned char PY_mb_ha    []={"¹ş¸òîş"};
const unsigned char PY_mb_hai   []={"»¹¿Èº¢º£º¦º¤º¥º§º¡àËëÜõ°"};
const unsigned char PY_mb_han   []={"º¹ººº°º®º¬º­º¯º±º¨ºµº¶º·º¸º©ºªº«º²º³÷ıº´ÚõİÕŞşå«êÏìÊñüò¥òÀ"};
const unsigned char PY_mb_hang  []={"ĞĞº»º¼º½¿ÔÏïãìç¬ñş"};
const unsigned char PY_mb_hao   []={"ºÂºÃºÄºÅê»ğ©ºÁºÆå°¸äº¾º¿ºÀºÑİïŞ¶àÆàãå©ò«òº"};
const unsigned char PY_mb_he    []={"ºÇºÈºÉºÊºËºÌºÍºÎºÏºÓºĞºÑºÒºÔºÕºØºÖº×ÏÅÚ­ÛÀÛÖàÀãØæüêÂîÁò¢òÂôç"};
const unsigned char PY_mb_hei   []={"ºÚºÙ"};
const unsigned char PY_mb_hen   []={"ºÛºÜºİºŞ"};
const unsigned char PY_mb_heng  []={"ºßºàºáºâºãŞ¿çñèì"};
const unsigned char PY_mb_hong  []={"ºìºäºåºæºçºèºéºêºëÙäÙêÚ§İ¦Ş®Ş°ãÈãü"};
const unsigned char PY_mb_hou   []={"ºóºíºîºïºğºñºòÜ©ááåËğúóóô×ö×÷¿"};
const unsigned char PY_mb_hu    []={"»¤»¥»¦»§ºËºôºõºöº÷ºøºùºúºû»¢ºüºıºş»¡»£Ùüßüàñá²â©âïã±ä°äïçúéÎéõì²ìÃìÎìæìèìïğ­ğÀğÉğ×ò®óËõ­õú÷½"};
const unsigned char PY_mb_hua   []={"»¨»ª»«»¬»©»­»®»¯»°æèèëí¹îü"};
const unsigned char PY_mb_huai  []={"»µ»±»²»³»´õ×"};
const unsigned char PY_mb_huan  []={"»¶»·»¸»¹»º»»»¼»½»¾»À»Á»Â»Ã÷ßä½»¿Û¨Û¼ÛùİÈß§à÷âµä¡äñå¾åÕçÙïÌöé"};
const unsigned char PY_mb_huang []={"»Î»Ä»Å»Æ»Ç»È»É»Ê»Ë»Ì»Í»Ï»Ğ»ÑÚòáåäÒäêåØè«ëÁñ¥ó¨óòöü"};
const unsigned char PY_mb_hui   []={"»á»Ø»Ù»Ú»Ò»Ó»Ô»Õ»Ö»×»Û»Ü»İ»Ş»ß»à»â»ã»ä»å»æåçÀ£Ú¶ÜîÜöŞ¥ßÔßÜà¹ãÄä§ä«çÀèíêÍí£ò³ó³÷â"};
const unsigned char PY_mb_hun   []={"»ç»è»é»ê»ë»ìÚ»âÆãÔäãçõ"};
const unsigned char PY_mb_huo   []={"ºÍ»í»î»ï»ğ»ñ»ò»ó»ô»õ»öØåŞ½ß«àëâ·îØïÁïìñëó¶"};
const unsigned char PY_mb_ji    []={"¼Æ¼Ç¸ø¼¼»÷»ù¼¸¼¹¼º»ø¼®¼¯¼°¼±¼²¼³¼´¼µ¼¶¼·¼»¼È¼É¼Ê¼½¼¾¼¿¼À¼Á»ú»û»ü»ı¼¡¼¢¼£¼¤¼¥¼¦¼§¼¨¼©¼ª¼«¼¬»ş¼­¼Â¼Ã¼Ä¼Å¼Ë¼Ì¼Í½åÆæÏµØ¢Ø½ØÀØŞÙ¥ÙÊÛÔÜ¸ÜÁÜùİ½İğŞªŞáß´ßÒßâßóá§áÕä©åæåìæ÷çÜçáé®éêêªê«êåêéê÷ì´í¶î¿ïúğ¢ñ¤ò±óÅóÇôßõÒõÕö«öİöê÷Ù÷ä"};
const unsigned char PY_mb_jia   []={"¼Ò¼Ó¼Î¼Ğ¼Ñ¼Ô¼Õ¼Ö¼×¼Ø¼Ù¼Ú¼Û¼Ü¼İ¼Ş¼ÏÇÑØÅÙ¤Û£İçáµä¤åÈçìê©ëÎí¢îòîşïØğèğıñÊòÌóÕôÂõÊ"};
const unsigned char PY_mb_jian  []={"½¨¼û¼ş¼ò½¡¼õ¼ß¼ö¼à¼ñ¼ó¼ô¼á¼â¼ä¼å¼æ¼ç¼è¼é¼ê¼ë¼ì¼í¼î¼ï¼ğ¼ã¼÷¼ø¼ù¼ú¼ü¼ı½¢½£½¤½¥½¦½§ÙÔÚÉÚÙÚÚİÑİóŞöàîäÕå¿åÀçÌèÅé¥ê§ê¯êğêùë¦ëìíúïµğÏñĞóÈôåõÂõİöä÷µ"};
const unsigned char PY_mb_jiang []={"½²ºç½©½ª½«½¬½­½®½¯½°½±½³½´½µÇ¿Üüä®ç­çÖêñíäñğôİôø"};
const unsigned char PY_mb_jiao  []={"½Ğ½Ï¾õ½ÇĞ£½¶½·½¸½¹½º½»½¼½½½¾½¿½À½Á½Â½Ã½Ä½Å½Æ½È½É½Ê½Ë½Ì½Í½Î½ÑÙ®ÙÕÜ´ÜúŞØàİá½áèæ¯ë¸ğ¨ğÔòÔõ´õÓöŞ"};
const unsigned char PY_mb_jie   []={"½à½á½â½ã½ä½Ò½Ü½Ó½Ô½ç½è½é½İ½Ş½Õ½Ö½×½Ø½Ù½Ú½Û½ß½å½æ½ê½ë½ìÙÊÚ¦ÚµÚàŞ×à®àµæ¼æİèîíÙïÇğÜò¡ò»ôÉöÚ÷º"};
const unsigned char PY_mb_jin   []={"½í¾¡½î½ï½ğ½ñ½ò½ô½õ½ö½÷½ø½ù½ú½û½ü½ı½ş¾¢½óÚáİ£İÀàäâËâÛæ¡çÆèªéÈêáêîîÄñÆñæ"};
const unsigned char PY_mb_jing  []={"¾©¾ª¾«¾­¾®¾¦¾§¾°¾»¾±¾²¾³¾´¾¯¾µ¾¶¾¹¾º¾¢¾£¾¤¾¥¾¨¾¬¾·¾¸ØÙÙÓÚåÚêİ¼â°ã½ãşåÉåòæºëÂëÖëæìºö¦"};
const unsigned char PY_mb_jiong []={"¾¼¾½ØçåÄìç"};
const unsigned char PY_mb_jiu   []={"¾Å¾Í¾È¾¾¾¿¾À¾Á¾Â¾Ã¾Ä¾Æ¾É¾Ê¾Ë¾Ì¾Î¾ÇÙÖà±ãÎèÑèêğ¯ğÕôñ÷İ"};
const unsigned char PY_mb_ju    []={"¾Ş¾ß¾à¾ã¾ä¾å³µ¹ñ¾ç¾Ó¾Ô¾Õ¾Ö¾×¾Ø¾Ù¾Ú¾Û¾Ü¾İ¾á¾â¾æ¾Ï¾Ğ¾Ñ¾ÒÙÆÚªÜÄÜÚÜìŞäåáåğè¢èÛé§é°é·éÙêøì«îÒï¸ñÀñÕôòõ¶õáö´öÂöÄ÷¶"};
const unsigned char PY_mb_juan  []={"¾ì¾í¾è¾é¾ê¾ë¾îÈ¦Û²áúä¸èğîÃïÃïÔöÁ"};
const unsigned char PY_mb_jue   []={"¾ö¾÷¾ø¾õ½Ç¾ò¾ó½À½Å¾ï¾ğ¾ñ¾ôØÊØãÚÜÛÇÜ¥Ş§àÙáÈâ±æŞçåèöéÓìßïãõêõû"};
const unsigned char PY_mb_jun   []={"¾ü¾ı¾ù¾ú¾û¾ş¿¡¿¢¿£¿¤¿¥¹êŞÜñäóŞ÷å"};
const unsigned char PY_mb_ka    []={"¿¨¿¦¿§¿©ØûßÇëÌ"};
const unsigned char PY_mb_kai   []={"¿ª¿«¿¬¿­¿®ØÜÛîİÜâéâıîøï´ïÇ"};
const unsigned char PY_mb_kan   []={"¿´¿¯¿°¿±¿²¿³Ù©íèî«ÛÉİ¨ãÛ¼÷ê¬"};
const unsigned char PY_mb_kang  []={"¿µ¿¶¿·¿¸¿¹¿º¿»ØøãÊîÖ"};
const unsigned char PY_mb_kao   []={"¿¼¿¿¿½¿¾èàêûîíåê"};
const unsigned char PY_mb_ke    []={"¿É¿Ê¿Ë¿Æ¿Ì¿Í¿Îà¾¿À¿Á¿Â¿Ã¿Ä¿Å¿Ç¿Èã¡òòá³äÛïıæìç¼çæéğë´î§îİîşºÇï¾ğâñ½ò¤òÂ÷Á"};
const unsigned char PY_mb_ken   []={"¿Ï¿Ğ¿Ñ¿ÒñÌ"};
const unsigned char PY_mb_keng  []={"¿Ó¿Ôï¬ëÖîï"};
const unsigned char PY_mb_kong  []={"¿Õ¿Ö¿×¿ØÙÅáÇóí"};
const unsigned char PY_mb_kou   []={"¿Ù¿Ú¿Û¿ÜÜÒŞ¢ßµíîóØ"};
const unsigned char PY_mb_ku    []={"¿İ¿Ş¿ß¿à¿á¿â¿ãç«÷¼ØÚÜ¥à·"};
const unsigned char PY_mb_kua   []={"¿ä¿å¿æ¿ç¿èÙ¨ï¾"};
const unsigned char PY_mb_kuai  []={"»á¿é¿ê¿ë¿ìØáÛ¦ßàáöëÚ"};
const unsigned char PY_mb_kuan  []={"¿í¿î÷Å"};
const unsigned char PY_mb_kuang []={"¿ö¿ï¿ğ¿ñ¿ò¿ó¿ô¿õÚ²Ú¿Ú÷ÛÛŞÅßÑæşêÜ"};
const unsigned char PY_mb_kui   []={"¿÷¿ø¿ù¿ú¿û¿ü¿ı¿şÀ¡À¢À£Ø¸ØÑåÓî¥õÍÙçÚóİŞŞñà­à°ã¦ã´êÒñùòñóñ"};
const unsigned char PY_mb_kun   []={"À¤À¥À¦À§ã§çûï¿õ«öïãÍ÷Õ"};
const unsigned char PY_mb_kuo   []={"À¨À©ÀªÀ«òÒ"};
const unsigned char PY_mb_la    []={"À¬À­À®À¯À°À±À²ÂäØİååê¹íÇğø"};
const unsigned char PY_mb_lai   []={"À³À´ÀµáÁáâäµäşêãíùñ®ô¥ïª"};
const unsigned char PY_mb_lan   []={"À¶À·À¸À¹ÀºÀ»À¼À½À¾À¿ÀÀÀÁÀÂÀÃÀÄá°äíé­ìµïçñÜî½"};
const unsigned char PY_mb_lang  []={"ÀÉÀÊÀËÀÅÀÆÀÇÀÈà¥ãÏòëï¶İõïüİ¹"};
const unsigned char PY_mb_lao   []={"ÀÌÀÍÀÎÀÏÀĞÀÑÀÒÀÓÀÔÂäÂçßëáÀõ²èáîîï©ğìñì"};
const unsigned char PY_mb_le    []={"ÀÕÀÖÁËØìß·ãî÷¦"};
const unsigned char PY_mb_lei   []={"ÀÛÀÕÀ×ÀØÀÙÀÚÀÜÀİÀŞÀßÀàÀáÙúÚ³àÏæĞçĞéÛñçõª"};
const unsigned char PY_mb_leng  []={"ÀâÀãÀäÜ¨ã¶"};
const unsigned char PY_mb_li    []={"ÀûÁ¦ÀúÀ÷Á¢Á£ÀñÁ¤ÀôÀíÀîÀïÀåÀøÀöÀæÀçÁ¥ÀèÀéÀëÀìÀğÀõÀùÀòÀóÀêÀüÀıÀşÁ¡Á§Á¨ØªÙ³ÙµÛªÛŞÜÂİ°İñŞ¼ß¿à¦à¬áûäàå¢åÎæ²æËæêçÊèÀèİéöìåíÂíÇîºî¾ï®ğ¿ğİğßòÃòÛó»óÒóöôÏõ·õÈö¨öâ÷¯÷ó"};
const unsigned char PY_mb_lia   []={"Á©"};
const unsigned char PY_mb_lian  []={"ÁªÁ«Á¬Á®Á¯Á°Á±Á²Á³Á´ÁµÁ¶Á·äòÁ­å¥çöé¬ŞÆéçİüì¡ñÍñÏó¹öã"};
const unsigned char PY_mb_liang []={"Á½ÁÁÁ©Á¸Á¹ÁºÁ»Á¼Á¾Á¿ÁÀÁÂÜ®İ¹é£õÔ÷Ë"};
const unsigned char PY_mb_liao  []={"ÁËÁÃÁÏÁÄÁÅÁÆÁÇÁÈÁÉÁÊÁÌÁÍÁÎŞ¤ŞÍàÚâ²å¼çÔîÉğÓ"};
const unsigned char PY_mb_lie   []={"ÁĞÁÑÁÒÁÓä£ÁÔÙıÛøŞæßÖôóõñ÷à"};
const unsigned char PY_mb_lin   []={"ÁÙÁÚÁŞÁàÁÕÁÖÁ×ÁØÁÛÁÜÁİÁßİşßøá×âŞãÁåàéİê¥ì¢î¬ôÔõï÷ë"};
const unsigned char PY_mb_ling  []={"ÁîÁéÁíÀâÁáÁâÁãÁäÁåÁæÁçÁèÁêÁëñöÁìÛ¹ÜßßÊàòãöç±èÚèùê²òÈôáöì"};
const unsigned char PY_mb_liu   []={"ÁõÁùÁïÁğÁñÁòÁóÁôÁöÁ÷ÁøÂµÂ½ä¯åŞæòç¸ì¼ìÖï³ïÖğÒöÌ"};
const unsigned char PY_mb_lo    []={"¿©"}; 
const unsigned char PY_mb_long  []={"ÁúÅªÁûÁüÁıÁşÂ¡Â¢Â£Â¤ÛâÜ×ãñççèĞëÊíÃñª"};
const unsigned char PY_mb_lou   []={"Â¥Â¦Â§Â¨Â©ÂªÂ¶ÙÍİäà¶áĞïÎğüñïò÷÷Ã"};
const unsigned char PY_mb_lu    []={"ÁùÂ«Â¬Â­Â®Â¯Â°Â±Â²Â³Â´ÂµÂ¶Â·Â¸Â¹ÂºÂ»Â¼Â½Â¾ÂÌÛäß£ààãòäËäõåÖè´èÓéÖéñéûê¤êÚëªëÍïåğµğØóüôµöÔ"};
const unsigned char PY_mb_lv    []={"ÂËÂÊÂÌÂÀÂÁÂÂÂÃÂÄÂÅÂÆÂÇÂÈÂÉÂ¿ñÚÙÍŞÛãÌéµëöïù"}; 
const unsigned char PY_mb_luan  []={"ÂÒÂÏÂÍÂÎÂĞÂÑ"};
const unsigned char PY_mb_lue   []={"ÂÓÂÔï²"};
const unsigned char PY_mb_lun   []={"ÂØÂÕÂÖÂ×ÂÙÂÚÂÛàğ"};
const unsigned char PY_mb_luo   []={"ÂäÂŞ¸õ¿©ÀÓÂÜÂİÂßÂàÂáÂâÂãÂåÂæÂçÙÀÙùÜıŞÛŞûâ¤ãøäğçóé¡ëáíÑïİñ§öÃ"};
const unsigned char PY_mb_m     []={"ß¼"};  
const unsigned char PY_mb_ma    []={"ÂèÂéÂêÂëÂìÂíÂîÂïÂğÄ¦Ä¨ßéáïæÖè¿ó¡"};
const unsigned char PY_mb_mai   []={"ÂñÂ÷ÂøÂùÂúÂûÂüÂıÂşÃ¡Ü¬á£çÏì×ïÜò©òı÷©÷´"};
const unsigned char PY_mb_man   []={"ÂùÂøÂ÷ÂúÂüÃ¡ÂıÂşÂû"};
const unsigned char PY_mb_mang  []={"Ã¢Ã£Ã¤Ã¥Ã¦Ã§ÚøäİíËòş"};
const unsigned char PY_mb_mao   []={"Ã¨Ã©ÃªÃ«Ã¬÷ÖÃ®Ã¯Ã°Ã±Ã²Ã³ÙóÃ­Üâá¹ã÷è£êÄêóë£ì¸í®î¦òúó±"};
const unsigned char PY_mb_me    []={"Ã´"};
const unsigned char PY_mb_mei   []={"Ã¿ÃÃÃÀÃµÃ¶ÃÁÃÂñÇ÷ÈÃ·Ã¸Ã¹ÃºÃ»Ã¼Ã½Ã¾ÃÄİ®áÒâ­ä¼äØé¹ïÑğÌ"};
const unsigned char PY_mb_men   []={"ÃÅÃÆÃÇŞÑìËí¯îÍ"};
const unsigned char PY_mb_meng  []={"ÃÈÃÎÃÉÃÊÃËÃÌÃÍÃÏÛÂİùŞ«ãÂëüíæòµòìó·ô»ô¿"};
const unsigned char PY_mb_mi    []={"ÃĞÃÑÃÒÃÓÃÔÃÕÃÖÃ×ÃØÃÙÃÚÃÛÃÜ÷çÚ×ÃİØÂÚ¢ŞÂßäà×â¨ãèåµåôæùëßìòôÍôé÷ã"};
const unsigned char PY_mb_mian  []={"ÃæÃŞÃßÃàÃáÃâÃãÃäÃåãæäÅäÏå²ëïíí"};
const unsigned char PY_mb_miao  []={"ÃçÃèÃéÃêÃëÃìÃíÃîß÷åãç¿çÑèÂíµíğğÅ"};
const unsigned char PY_mb_mie   []={"ÃïÃğØ¿ßãóúóº"};
const unsigned char PY_mb_min   []={"ÃñÃòÃóÃôÃõÃöÜåáºãÉãıçÅçäçëíªö¼÷ª"};
const unsigned char PY_mb_ming  []={"Ã÷ÃøÃùÃúÃûÃüÚ¤ÜøäéêÔî¨õ¤"};
const unsigned char PY_mb_miu   []={"ÃıçÑ"};
const unsigned char PY_mb_mo    []={"ÂöÃ»ÃşÄ¡Ä¢Ä£Ä¤Ä¥Ä¦Ä§Ä¨Ä©ÄªÄ«Ä¬Ä­Ä®Ä¯Ä°÷áÍòÚÓÜÔİëâÉæÆéâïÒï÷ñ¢ñòõöõø"};
const unsigned char PY_mb_mou   []={"Ä±Ä²Ä³Ù°ßèçÑíøòÖöÊ"};
const unsigned char PY_mb_mu    []={"Ä¾Ä¿ÄÀÄ£Ä²Ä´ÄµÄ¶Ä·Ä¸Ä¹ÄºÄ»Ä¼Ä½ÄÁÄÂÜÙãåØïÛéë¤îâ"};
const unsigned char PY_mb_na    []={"ÄÃÄÄÄÅÄÆÄÇÄÈÄÉŞàëÇïÕñÄ"};
const unsigned char PY_mb_nai   []={"ÄÊÄËÄÌÄÍÄÎØ¾Ù¦ÜµİÁèÍ"};
const unsigned char PY_mb_nan   []={"ÄÏÄĞÄÑà«àïéªëîòïôö"};
const unsigned char PY_mb_nang  []={"ÄÒàìâÎß­êÙ"};
const unsigned char PY_mb_nao   []={"ÄÓÄÔÄÕÄÖÄ×Ø«ÛñßÎâ®è§íĞîóòÍ"};
const unsigned char PY_mb_ne    []={"ÄÄÄØÚ«"};
const unsigned char PY_mb_nei   []={"ÄÚÄÙ"};
const unsigned char PY_mb_nen   []={"ÄÛí¥"};
const unsigned char PY_mb_neng  []={"ÄÜ"};
const unsigned char PY_mb_ng    []={"àÅ"}; 
const unsigned char PY_mb_ni    []={"ÄãÄØÄâÄáÄİÄŞÄßÄàÄäÄåÄæÄçÙ£Ûèâ¥âõêÇì»ìòí«íşîêöò"};
const unsigned char PY_mb_nian  []={"ÄèÄéÄêÄëÄìÄíÄîØ¥Ûşéığ¤öÓöó"};
const unsigned char PY_mb_niang []={"ÄïÄğ"};
const unsigned char PY_mb_niao  []={"ÄñÄòëåôÁÜàæÕ"};
const unsigned char PY_mb_nie   []={"ÄóÄôÄõÄöÄ÷ÄøÄùõæŞÁà¿Ø¿Úíò¨ô«"};
const unsigned char PY_mb_nin   []={"Äú"};
const unsigned char PY_mb_ning  []={"ÄûÄüÄıÄşÅ¡Å¢ØúßÌå¸ñ÷"};
const unsigned char PY_mb_niu   []={"Å£Å¤Å¥Å¦ŞÖæ¤áğâî"};
const unsigned char PY_mb_nong  []={"Å§Å¨Å©ÅªÙ¯ßæ"};
const unsigned char PY_mb_nou   []={"ññ"};
const unsigned char PY_mb_nu    []={"Å«Å¬Å­åóæÀæÛæå"};
const unsigned char PY_mb_nuan  []={"Å¯"};
const unsigned char PY_mb_nue   []={"Å±Å°"};
const unsigned char PY_mb_nuo   []={"ÄÈÅ²Å³Å´ÅµÙĞŞùßöï»"};
const unsigned char PY_mb_nv    []={"Å®í¤îÏô¬"};
const unsigned char PY_mb_o     []={"Å¶à¸àŞ"};
const unsigned char PY_mb_ou    []={"Å·Å¸Å¹ÅºÅ»Å¼Å½ÇøÚ©âæê±ñî"};
const unsigned char PY_mb_pa    []={"°Ç°ÒÅ¾Å¿ÅÀÅÁÅÂÅÃİâèËóá"};
const unsigned char PY_mb_pai   []={"ÅÄÅÅÅÆÅÇÅÈÅÉÆÈÙ½İåßß"};
const unsigned char PY_mb_pan   []={"·¬ÅÊÅËÅÌÅÍÅÎÅÏÅĞÅÑó´õçÅÖŞÕãÜãİãúêÚñÈñá"};
const unsigned char PY_mb_pang  []={"°ò°õ°÷ÅÒÅÓÅÔÅÕÅÖáİäèó¦åÌ"};
const unsigned char PY_mb_pao   []={"Å×ÅØÅÙÅÚÅÛÅÜÅİáóâÒğåëãŞË"};
const unsigned char PY_mb_pei   []={"ÅŞÅßÅàÅáÅâÅãÅäÅåÅæàÎö¬àúì·ïÂõ¬"};
const unsigned char PY_mb_pen   []={"ÅçÅèäÔ"};
const unsigned char PY_mb_peng  []={"ÅõÅöÅéÅêÅëÅìÅíÅîÅïÅğÅñÅòÅóÅôâñÜ¡àØó²"};
const unsigned char PY_mb_pi    []={"±Ù·ñÅ÷ÅøÅùÅúÅûÅüÅıÅşÆ¡Æ¢Æ£Æ¤Æ¥Æ¦Æ§Ø§Æ¨Æ©ñ±õùØòÚéÚğÚüÛ¯ÛÜÛıÜ±ÜÅß¨àèâÏäÄæÇç¢èÁê¶î¢î¼îëñÔñâò·òç"};
const unsigned char PY_mb_pian  []={"±â±ãÆªÆ«Æ¬Æ­ÚÒæéçÂêúëİôæõä"};
const unsigned char PY_mb_piao  []={"Æ®Æ¯Æ°Æ±ÆÓØâæÎçÎî©àÑæôéèóª"};
const unsigned char PY_mb_pie   []={"Æ²Æ³Ø¯ÜÖë­"};
const unsigned char PY_mb_pin   []={"Æ´ÆµÆ¶Æ·Æ¸æ°æÉé¯êòò­"};
const unsigned char PY_mb_ping  []={"·ëÆ¹ÆºÆ»Æ¼Æ½Æ¾Æ¿ÆÀÆÁÙ·æ³èÒöÒ"};
const unsigned char PY_mb_po    []={"²´·±ÆÂÆÃÆÄÆÅÆÆÆÇÆÈÆÉÆÓØÏÚéÛ¶çêê·ë¶îÇîŞğ«óÍ"};
const unsigned char PY_mb_pou   []={"ÆÊÙöŞå"};
const unsigned char PY_mb_pu    []={"ÆÕ±¤±©¸¬ÆËÆÌÆÍÆÎÆÏÆĞÆÑÆÒÆÓÆÔÆÖÆ×ÆØÆÙÙéàÛäßõëå§è±ë«ïäïè"};
const unsigned char PY_mb_qi    []={"»ü¼©ÆÚÆÛÆÜÆİÆŞÆßÆàÆáÆâÆãÆäÆåÆæÆçÆèÆéÆêÆëÆìÆíáªÆîÆïÆğÆñÆòÆóÆôç÷çùè½ÆõÆöÆ÷ÆøÆùÆúÆûÆüÆıØ½ì÷í¬õè÷¢÷èØÁÙ¹ÛßÜ»ÜÎÜùİÂİİä¿æëç²êÈŞ­èçéÊì¥àÒá¨ãàíÓñıòÓòàôëôì"};
const unsigned char PY_mb_qia   []={"¿¨ÆşÇ¡Ç¢İÖñÊ÷Ä"};
const unsigned char PY_mb_qian  []={"Ç£Ç¤Ç¥Ç¦Ç§Ç¨Ç©ÇªÇ«Ç¬Ç­Ç®Ç¯Ç°Ç±Ç²Ç³Ç´ÇµÇ¶Ç·Ç¸ÏËÙ»ÙİÚäÜ·ÜÍÜçİ¡Şçá©ã¥ã»å¹åºå½ç×èıêùëÉí©îÔò¯óéôÇ"};
const unsigned char PY_mb_qiang []={"Ç¹ÇºÇ»Ç¼Ç½Ç¾Ç¿ÇÀõÄãŞê¨ìÁïÏïêãÜñßæÍéÉòŞôÇ"};
const unsigned char PY_mb_qiao  []={"¿ÇÇÁÇÂÇÃÇÄÇÅÇÆÇÇÇÈÇÉÇÊÇËÇÌÇÍÇÎÇÏÈ¸Ú½ÚÛÜñá½ã¾éÔõÎ÷³çØØäã¸íÍï¢"};
const unsigned char PY_mb_qie   []={"ÇĞÇÑÇÒÇÓÇÔÙ¤ã«ã»æªôòÛ§êüïÆóæ"};
const unsigned char PY_mb_qin   []={"ÇÕÇÖÇ×ÇØÇÙÇÚÇÛÇÜÇİÇŞÇßÜËŞìôÀßÄàºàßâÛéÕï·ñæñûòû"};
const unsigned char PY_mb_qing  []={"Ç×ÇàÇáÇâÇãÇäÇåÇæÇçÇèÇéÇêÇëÇìíàÜÜàõéÑïºòßóÀóäôìö¥öë÷ô"};
const unsigned char PY_mb_qiong []={"ÇíÇîñ·òËÚöÜäóÌõ¼öÆ"};
const unsigned char PY_mb_qiu   []={"³ğ¹êÇïÇğÇñÇòÇóÇôÇõÇöÙ´åÏôÃôÜåÙÛÏáìäĞé±êäò°òÇòøöú"};
const unsigned char PY_mb_qu    []={"Ç÷ÇøÇùÇúÇûÇüÇıÇşÈ¡È¢È£È¤È¥òĞ÷ñĞçÚ°Û¾ÛÉÜÄŞ¡Ş¾á«áéãÖè³êïë¬ëÔìîíáğ¶ñ³ó½ôğöÄ"};
const unsigned char PY_mb_quan  []={"È¦È§È¨È©ÈªÈ«È¬È­È®È¯È°Ú¹Üõòé÷Üáëãªç¹éúî°îıóÜ"};
const unsigned char PY_mb_que   []={"È±È²È³È´ÈµÈ¶È·È¸ã×ãÚí¨"};
const unsigned char PY_mb_qui   []={"÷ü"};  
const unsigned char PY_mb_qun   []={"È¹ÈºåÒ÷å"};
const unsigned char PY_mb_ran   []={"È»È¼È½È¾ÜÛòÅ÷×"};
const unsigned char PY_mb_rang  []={"È¿ÈÀÈÁÈÂÈÃìüğ¦"};
const unsigned char PY_mb_rao   []={"ÈÄÈÅÈÆÜéæ¬èã"};
const unsigned char PY_mb_re    []={"ÈÇÈÈßö"};
const unsigned char PY_mb_ren   []={"ÈÉÈÊÈËÈÌÈÍÈÎÈÏÈĞÈÑÈÒïşØéØğÜóİØâ¿éíñÅ"};
const unsigned char PY_mb_reng  []={"ÈÓÈÔ"};
const unsigned char PY_mb_ri    []={"ÈÕ"};
const unsigned char PY_mb_rong  []={"ÈÖÈ×ÈØÈÙÈÚÈÛÈÜÈİÈŞÈßáÉáõéÅëÀòî"};
const unsigned char PY_mb_rou   []={"ÈàÈáÈâôÛõå÷·"};
const unsigned char PY_mb_ru    []={"ÈãÈäÈåÈæÈçÈèÈéÈêÈëå¦ÈìäáİêàéŞ¸ä²çÈï¨ñàò¬"};
const unsigned char PY_mb_ruan  []={"ÈíÈîëÃ"};
const unsigned char PY_mb_rui   []={"ÈïÈğÈñÜÇî£èÄŞ¨ò¸"};
const unsigned char PY_mb_run   []={"ÈòÈó"};
const unsigned char PY_mb_ruo   []={"ÈôÈõÙ¼óè"};
const unsigned char PY_mb_sa    []={"ÈöÈ÷ÈøØ¦ØíìªëÛ"};
const unsigned char PY_mb_sai   []={"ÈùÈúÈûÈüàç"};
const unsigned char PY_mb_san   []={"ÈıÈşÉ¡É¢âÌë§ôÖ"};
const unsigned char PY_mb_sang  []={"É£É¤É¥Şúíßòª"};
const unsigned char PY_mb_sao   []={"É¦É§É¨É©ëığşÜ£çÒöş"};
const unsigned char PY_mb_se    []={"ÈûÉªÉ«É¬ØÄï¤ğ£"};
const unsigned char PY_mb_sen   []={"É­"};
const unsigned char PY_mb_seng  []={"É®"};
const unsigned char PY_mb_sha   []={"É¯É°É±É²É³É´ÉµÉ¶É·É¼ÏÃğğôÄö®öèßşì¦ï¡"};
const unsigned char PY_mb_shai  []={"É«É¸É¹"};
const unsigned char PY_mb_shan  []={"²ôµ¥ÉºÉ»É¼É½É¾É¿ÉÀÉÁÉÂÉÃÉÄÉÅÉÆÉÇÉÈÉÉÕ¤Ú¨ğŞô®óµõÇ÷­ØßÛ·ÛïÜÏáêäúæ©æÓæóëşìøîÌ÷Ô"};
const unsigned char PY_mb_shang []={"ÉÊÉËÉÌÉÍÉÎÉÏÉĞÉÑç´éäõüÛğìØ"};
const unsigned char PY_mb_shao  []={"ÇÊÉÒÉÓÉÔÉÕÉÖÉ×ÉØÉÙÉÚÉÛÉÜÛ¿Üæô¹äûè¼òÙóâ"};
const unsigned char PY_mb_she   []={"ÉİÉŞÉßÉàÉáÉâÉãÉäÉåÉæÉçÉè÷êÕÛØÇÙÜŞéâ¦äÜì¨î´îè"};
const unsigned char PY_mb_shei  []={"Ë­"}; 
const unsigned char PY_mb_shen  []={"²ÎÉéÉêÉëÉìÉíÉîÉïÉğÉñÉòÉóÉôÉõÉöÉ÷ÉøÊ²Ú·ÚÅİ·İØßÓäÉé©ëÏïòò×"};
const unsigned char PY_mb_sheng []={"³ËÉùÉúÉûÉüÉıÉşÊ¡Ê¢Ê£Ê¤Ê¥óÏáÓäÅêÉíò"};
const unsigned char PY_mb_shi   []={"ÊÇÊÂ³×Ê¦Ê®Ê¯Ê§Ê¨Ê©ÊªÊ«Ê¬Ê­Ê°Ê±Ê²Ê³Ê´ÊµÊ¶Ê·Ê¸Ê¹ÊºÊ»Ê¼Ê½Ê¾Ê¿ÊÀÊÁÊÃÊÄÊÅÊÆÊÈÊÉÊÊÊËÊÌÊÍÊÎÊÏÊĞÊÑÊÒÊÓÊÔËÆß±óÂÖ³ÖÅÚÖÛõİªİéâ»éøêÛìÂìêîæîèó§óßõ§õ¹öåöõ"};
const unsigned char PY_mb_shou  []={"ÊÕÊÖÊ×ÊØÊÙÊÚÊÛÊÜÊİÊŞŞĞá÷ç·ô¼"};
const unsigned char PY_mb_shu   []={"ÊßÊàÊáÊâÊãÊäÊåÊæÊçÊèÊéÊêÊëÊìÊíÊîÊïÊğÊñÊòÊóÊôÊõÊöÊ÷ÊøÊùÊúÊûÊüÊıÊşË¡Ù¿ÛÓãğäøæ­Ø­İÄŞóç£ë¨ëòì¯ïíïøñâ"};
const unsigned char PY_mb_shua  []={"Ë¢Ë£à§"};
const unsigned char PY_mb_shuai []={"ÂÊË¤Ë¥Ë¦Ë§ó°"};
const unsigned char PY_mb_shuan []={"Ë¨Ë©ãÅäÌ"};
const unsigned char PY_mb_shuang[]={"ËªË«Ë¬ãñæ×"};
const unsigned char PY_mb_shui  []={"Ë®Ë¯Ë°Ëµãß"};
const unsigned char PY_mb_shun  []={"Ë±Ë²Ë³Ë´"};
const unsigned char PY_mb_shuo  []={"ÊıËµË¶Ë·Ë¸İôŞ÷åùéÃîå"};
const unsigned char PY_mb_si    []={"Ë¹ËºË»Ë¼Ë½Ë¾Ë¿ËÀËÁËÂËÃËÄËÅËÆËÇËÈØËßĞÙ¹ÙîÛÌâ»ãáãôäùæ¦æáçÁìëïÈğ¸ñêòÏóÓ"};
const unsigned char PY_mb_song  []={"ËÉËÊËËËÌËÍËÎËÏËĞáÔâìã¤äÁñµáÂİ¿Ú¡"};
const unsigned char PY_mb_sou   []={"ËÑËÒËÓËÔÛÅà²âÈäÑì¬î¤ïËòôàÕŞ´"};
const unsigned char PY_mb_su    []={"ËÕËÖË×ËØËÙËÚËÛËÜËİËŞËßËàËõÙíãºä³óùöÕÚÕİøà¼ö¢"};
const unsigned char PY_mb_suan  []={"ËáËâËãâ¡"};
const unsigned char PY_mb_sui   []={"ËäËêËçËåËæËèËîËìËéËíËë"};
const unsigned char PY_mb_sun   []={"ËïËğËñ"};
const unsigned char PY_mb_suo   []={"ËòËóËôËõËöË÷ËøËùßïàÂàÊæ¶É¯èøêıíüôÈ"};
const unsigned char PY_mb_ta    []={"ËúËûËüËıËşÌ¡Ì¢í³Ì£Ì¤ÍØãËåİé½îèõÁäâ÷£äğ"};
const unsigned char PY_mb_tai   []={"Ì¥Ì¦Ì§Ì¨Ì©ÌªÌ«Ì¬Ì­õÌÛ¢Ş·ææëÄìÆîÑöØ"};
const unsigned char PY_mb_tan   []={"µ¯Ì®Ì¯Ì°Ì±Ì²Ì³Ì´ÌµÌ¶Ì·Ì¸Ì¹ÌºÌ»Ì¼Ì½Ì¾Ì¿ñûÛ°å£ê¼êæìşîãïÄïâ"};
const unsigned char PY_mb_tang  []={"ÌÀÌÁÌÂÌÃÌÄÌÅÌÆÌÇÌÈÌÉÌÊÌËÌÌÙÎâ¼äçè©éÌîõó«ï¦ïÛñíó¥ôÊõ±àû"};
const unsigned char PY_mb_tao   []={"ÌÍÌÎÌÏÌĞÌÑÌÒÌÓÌÔÌÕÌÖÌ×ß¶ßû÷Òä¬èºìâØ»"};
const unsigned char PY_mb_te    []={"ÌØß¯ìıï«"};
const unsigned char PY_mb_teng  []={"ÌÙÌÚÌÛëøÌÜ"};
const unsigned char PY_mb_ti    []={"ÌİÌŞÌßÌàÌáÌâÌãÌäÌåÌæÌçÌèÌéÌêÌëÙÃã©ç¾ÜèåÑç°ğÃñÓõ®"};
const unsigned char PY_mb_tian  []={"ÌìÌíÌîÌïÌğÌñÌòÌóéåŞİãÃãÙî±îäï»"};
const unsigned char PY_mb_tiao  []={"µ÷ÌôÌõÌöÌ÷ÌøÙ¬÷ØÜæìöï¢ñ»òèóÔôĞö¶öæ"};
const unsigned char PY_mb_tie   []={"ÌùÌúÌû÷Ñï°İÆ"};
const unsigned char PY_mb_ting  []={"ÌüÌıÌşÍ¡Í¢Í£Í¤Í¥Í¦Í§ÜğİãæÃîúòÑöªèè"};
const unsigned char PY_mb_tong  []={"Í©Í«Í¬Í­Í®Í¯Í°Í±Í²Í³Í´Ù¡Ù×ÜíâúäüÍªÙÚÛíàÌá¼íÅ"};
const unsigned char PY_mb_tou   []={"ÍµÍ¶Í·Í¸÷»î×Ùï"};
const unsigned char PY_mb_tu    []={"Í¹ÍºÍ»Í¼Í½Í¾Í¿ÍÀÍÁÍÂÍÃÜ¢İ±İËîÊõ©"};
const unsigned char PY_mb_tuan  []={"ÍÄÍÅŞÒåèî¶"};
const unsigned char PY_mb_tui   []={"ÍÆÍÇÍÈÍÉÍÊÍËìÕ"};
const unsigned char PY_mb_tun   []={"¶ÚÍÊÍÌÍÍÍÎâ½ëàÙÛêÕ"};
const unsigned char PY_mb_tuo   []={"ËµÍÏÍØÍĞÍÑÍÒÍÓÍÔÍÕÍÖÍ×ÍÙõÉØ±Ù¢ÛçãûèØèŞéÒíÈîèö¾óêõ¢âÕ"};
const unsigned char PY_mb_wa    []={"ÍÚÍÛÍÜÍİÍŞÍßÍàØôæ´ëğ"};
const unsigned char PY_mb_wai   []={"ÍáÍâáË"};
const unsigned char PY_mb_wan   []={"ÂûÍãÍäÍåÍæÍçÍèÍéÍêÍëÍìÍíÍîÍïÍğÍñÍòÍóØàÜ¹İ¸İÒæıòêçºçşëäîµ÷´"};
const unsigned char PY_mb_wang  []={"ÍôÍõÍöÍ÷ÍøÍùÍúÍûÍüÍıØèã¯÷ÍŞÌéş"};
const unsigned char PY_mb_wei   []={"ÎªÎ»Î½ÍşÎ¡Î¢Î£Î¤Î¬Î¥Î¦Î§Î¨Î©Î«Î­Î®Î¯Î°Î±Î²Î³Î´ÎµÎ¶Î·Î¸Î¹ÎºÎ¼Î¾Î¿ÎÀÙËÚÃÚóÛ×İÚŞ±àøá¡áËáÍâ¬åÔæ¸ğôì¿çâãÇâ«Úñãíä¢ä¶è¸ê¦ìĞôºöÛàí"};
const unsigned char PY_mb_wen   []={"ÎÊÎÄÎÅÎÆÎÁÎÂÎÃÎÇÎÈÎÉØØö©çäãÓãëè·"};
const unsigned char PY_mb_weng  []={"ÎËÎÌÎÍİîŞ³"};
const unsigned char PY_mb_wo    []={"ÎÒÎÕà¸ÎÏÎĞÎÑÎÓÎÔÎÖÎÎÙÁİ«á¢ö»ä×ë¿íÒ"};
const unsigned char PY_mb_wu    []={"ÎåÎïÎğÎñÎçÎèÎéÎäÎê¶ñÎòÎóØ£Î×ÎíÎØÎÙÎÚÎÛÎÜÎİÎŞÎßÎàÎáÎâÎãÎæÎëÎìÎîØõÚùğÄÛØÜÌßíâĞâäâèğÍğíòÚä´å»åÃåüæÄæğÚãè»êõì¶ìÉöÈ÷ù"};
const unsigned char PY_mb_xi    []={"Ï£Ï°Ï¦Ï·Ï¸ÎôÎõÎöÎ÷ÎøÎùÎúÎûÎüÎıÎşÏ¡õèÏ¢Ï¤Ï¥Ï§Ï¨ÏªÏ«Ï¬Ï®Ï¯Ï±Ï²Ï´ÏµÏ¶ôËğªåïæÒçôéØŞÉÜçÏ­Ï©Ï³ØÎÙÒÙâÚôÛ­İ¾İßİûßñáãâ¾ãÒä»äÀêØêêì¤ì¨ìäìùìûñ¶ñÓòáó£ó¬ôªô¸ôÑôâõµ÷û"};
const unsigned char PY_mb_xia   []={"Ï¹ÏºÏ»Ï¼Ï½Ï¾Ï¿ÏÀÏÁÏÂÏÃÏÄÏÅßÈáòåÚè¦èÔíÌóÁ÷ï"};
const unsigned char PY_mb_xian  []={"ÏÖÏÈÏÉÏÓÏÔÏÕÏ×ÏØÏÍÏ³Ï´ÏÆÏÇÏÊÏËÏÌÏÎÏÏÏĞÏÑÏÒÏÙÏÚÏÛÏÜÏİÏŞÏßÙşÜÈİ²Şºá­áıåßæµë¯ìŞììğÂğçğïò¹óÚôÌõ£õĞõÑö±"};
const unsigned char PY_mb_xiang []={"ÏñÏòÏó½µÏàÏáÏâÏãÏäÏæÏçÏèÏéÏêÏëÏìÏíÏîÏïÏğÏå÷ÏÜ¼İÙâÃâÔæøç½ó­öß"};
const unsigned char PY_mb_xiao  []={"Ğ¡ÏûĞ¤ÏôÏõÏöÏ÷ÏøÏùÏúÏüÏıÏşĞ¢Ğ£Ğ¥Ğ¦Ğ§ßØäìåĞóãóïæçç¯èÉèÕòÙ÷Ì"};
const unsigned char PY_mb_xie   []={"Ğ©ĞªĞ«Ğ¬Ğ­Ğ®Ğ¯Ğ°Ğ±Ğ²Ğ³Ğ´ĞµĞ¶Ğ·Ğ¸Ğ¹ĞºĞ»Ğ¼Ñª½âĞ¨Ò¶ÙÉÙôÛÄÛÆŞ¯ß¢â³âİäÍå¬åâç¥çÓé¿éÇò¡õóöÙ÷º"};
const unsigned char PY_mb_xin   []={"Ğ½Ğ¾Ğ¿ĞÀĞÁĞÂĞÃĞÄĞÅĞÆÜ°İ·ì§öÎê¿Ø¶âàïâ"};
const unsigned char PY_mb_xing  []={"ĞÒÊ¡ĞÇĞÈĞÉĞÊĞËĞÌĞÍĞÎĞÏĞĞĞÑĞÓĞÔĞÕâ¼ÚêÜôÜşß©ã¬íÊ"};
const unsigned char PY_mb_xiong []={"ĞÖĞ×ĞØĞÙĞÚĞÛĞÜÜº"};
const unsigned char PY_mb_xiu   []={"³ôËŞĞİĞŞĞßĞàĞáĞâĞãĞäĞåßİäåõ÷âÊá¶âÓğ¼÷Û"};
const unsigned char PY_mb_xu    []={"ĞøĞ÷ĞæĞçĞèĞéĞêĞëĞìĞíĞîĞïĞğĞñĞòĞóĞôĞõĞöÚ¼ìãÛÃÛ×Ş£äªä°äÓçïèòÓõñãíìôÚõ¯"};
const unsigned char PY_mb_xuan  []={"ĞşÑ¡ĞùĞúĞûäÖĞüĞıêÑÑ¤ìÅÑ£Ñ¢ÙØÚÎİæãùäöè¯é¸ìÓŞïíÛîçïàÈ¯"};
const unsigned char PY_mb_xue   []={"Ï÷Ñ¥Ñ¦Ñ§Ñ¨Ñ©Ñª÷¨ÚÊàåí´õ½"};
const unsigned char PY_mb_xun   []={"Ñ°Ñ®Ñ¸Ñ¶Ñ¯ÑµÑ«Ñ¬Ñ­Ñ±Ñ²Ñ³Ñ´Ñ·Û¨Û÷Ü÷Ş¹á¾áßõ¸öà¿£İ¡Ş¦â´Ùãâşä­ä±êÖñ¿"};
const unsigned char PY_mb_ya    []={"ÑÀÑ¿ÑÇÑÆÑ¹ÑÅÑºÑ»Ñ¼Ñ½Ñ¾ÑÂÑÃÑÄÑÈÔşØóÑÁÛëŞëá¬åÂæ«çğèâë²í¼íığéñâ"};
const unsigned char PY_mb_yan   []={"ÑÔÑİÑéÑáÑÏÑØÑ×ÑàÑÊÑËÑÙÑÚÑÛÑÌÑÍÑÎÑĞÑÒÑÓÑÕÑÖÑÜÑŞÑßÑâÑãÑäÑÉÑåÑæÑçÑèÑÑãÆåûæÌëÙëçìÍâûÙ²ÙÈÇ¦ÒóØÉØÍØßÙğÚ¥ÚİÚçÛ±Û³Ü¾İÎáÃ÷ÊãÕäÎäÙçüéÜêÌî»óÛõ¦÷Ğ÷ú÷ü"};
const unsigned char PY_mb_yang  []={"ÑëÑöÑòÑóÑôÑøÑùÑîÑïí¦ÑêÑìÑíÑğÑõÑñÑ÷Ñúáàâóãóì¾ìÈòÕ÷±"};
const unsigned char PY_mb_yao   []={"ÒªÑûÒ§Ò©ÑüÑıÒ¡Ò¢ëÈñºÒ£Ò¤Ò¥Ò¦Ò¨Ò«Ô¿Ø²Ø³çÛßºÑşÄöÅ±½ÄáÅáÊáæçòèÃé÷ê×ï¢ğÎôí÷¥"};
const unsigned char PY_mb_ye    []={"Ò²Ò³ÒµÒ¶Ò¹ÒºÒ¬Ò­Ò®Ò¯Ò°Ò±Ò´Ò·Ò¸Ğ°ÑÊÚËÖÑØÌÚşŞŞçğêÊìÇîô"};
const unsigned char PY_mb_yi    []={"Ò»ÒÒÒÑÒÔÒäÒåÒéÒêÒëÒìÒæÒ¼Ò½Ò¾Ò¿ÒÀÒÁÒÂÒÃÒÄÒÅÒÆÒÇÒÈÒÉÒÊÒËÒÌÒÍÒÎÒÏÒĞÒÓÒÕÒÖÒ×ÒØÒÙÒÚÒÛÒÜÒİÒŞÒßÒàÒáÒâÒãÒçÒèÒíÒî°¬Î²ÒïØ×ØæØıÙ«Ú±ÚâÛİÛüÜ²ÜÓÜèŞ²ŞÄŞÈŞÚß®ß½ß×ßŞàæá»áÚâ¢âÂâøâùã¨äôåÆæäçËéìéóêİì¥ì½ìÚíôîÆîèï×ïîğêğùñ¯ñ´ñÂòæô¯ôàôèôı÷ğ"};
const unsigned char PY_mb_yin   []={"ÒüÒıÓ¡ÒğÒñÒòÒóÒôÒõÒöÒ÷ÒøÒùÒúÒûÒşö¸Ø·ò¾Û´ÛÈÛóÜ§ÜáßÅà³áşâ¹ä¦äÎë³î÷ñ«ñ¿ö¯"};
const unsigned char PY_mb_ying  []={"Ó³Ó°Ó¢Ó¨Ó©ÓªÓ«Ó£Ó¤Ó¥Ó¦Ó§Ó¬Ó­Ó®Ó¯Ó±Ó²ÙøİºİÓğĞäëå­äŞçøÛ«ÜãÜşİöŞüàÓâßè¬éºëôñ¨ò£ó¿"};
const unsigned char PY_mb_yo    []={"Ó´Óıà¡"};
const unsigned char PY_mb_yong  []={"ÓÃÓµÓ¶ÓÂÓ¿Ó·Ó¸Ó¹ÓºÓ»Ó¼Ó½Ó¾ÓÀÓÁÙ¸ÛÕÜ­à¯ã¼çßïŞğ®÷«÷Ó"};
const unsigned char PY_mb_you   []={"ÓÈÓĞÓÖÓÕÓ×ÓÑÓÒÓÓÓÄÓÅÓÆÓÇÓÉÓÊÓËÓÌÓÍÓÎÓÏÓÔßÏğàöÏ÷î÷øòÄØüØÕÙ§İ¬İ¯İµŞÌàóå¶èÖéàë»îğòÊòöòøôí"};
const unsigned char PY_mb_yu    []={"ÓàÓëÓÚÓèÓîÓñÓêÓıÓşÔ¡ÓãÓØÓÙÓÛÓİÓŞÓßÓáÓâÓäÓåÓæÓçÓéÓìÓíÓïÓğÓòÓóÓôÓõÓöÓ÷ÓøÓùÓúÓÜÓûÓüÔ¢Ô£Ô¤Ô¥Ô¦óÄØ¹ÚÄì¶ÚÍÎµÎ¾Ø®ØñÙ¶İÇİÒİ÷ŞíàôàöáÎáüâÀâÅâ×ãĞå÷åıæ¥æúè¤êÅêìëéì£ìÏìÙìÛí±í²îÚğÁğÖğõğöñ¾ñÁòâòõô§ô¨ö§ö¹"};
const unsigned char PY_mb_yuan  []={"Ô­ÔªÔ°Ô±Ô²Ô´ÔµÔ¶Ô¸Ô¹ÔºÔ§Ô¨Ô©Ô«Ô¬Ô®Ô¯Ô³Ô·æÂÜ«Ü¾Şòà÷ãäè¥éÚë¼íóğ°ó¢óîö½"};
const unsigned char PY_mb_yue   []={"ÔÂÔÃÔÄÀÖËµÔ»Ô¼Ô½Ô¾Ô¿ÔÀÔÁÙßßÜå®èİéĞë¾îá"};
const unsigned char PY_mb_yun   []={"ÔÆÔ±ÔÊÔËÔÎÔÅÔÈÔÉÔÌÔÍìÙÔÏÔĞëµéæÜ¿ÔÇÛ©áñã¢ã³ç¡è¹êÀóŞ"};
const unsigned char PY_mb_za    []={"ÔÑÔÒÔÓÔúÕ¦ßÆŞÙ"};
const unsigned char PY_mb_zai   []={"ÔÙÔÚÔÔÔÕÔÖÔ×ÔØáÌçŞ"};
const unsigned char PY_mb_zan   []={"ÔÛÔÜÔİÔŞêÃô¢ŞÙè¶ôØôõöÉ"};
const unsigned char PY_mb_zang  []={"²ØÔßÔàÔáæàê°"};
const unsigned char PY_mb_zao   []={"ÔæÔâÔçÔìÔíÔîÔãÔäÔèÔêÔëÔïÔåÔéßğ"};
const unsigned char PY_mb_ze    []={"ÔğÔñÔòÔóÕ¦ØÆØÓßõàıåÅê¾óĞóåô·"};
const unsigned char PY_mb_zei   []={"Ôô"};
const unsigned char PY_mb_zen   []={"ÔõÚÚ"};
const unsigned char PY_mb_zeng  []={"ÔöÔ÷ÔøÔùçÕêµîÀï­"};
const unsigned char PY_mb_zha   []={"ÔúÕ¢²éÔûÔüÔıÔşÕ¡Õ£Õ¤Õ¥Õ¦Õ§Õ¨Õ©ß¸ßå×õŞêòÆßîà©é«íÄğäö´÷ş"};
const unsigned char PY_mb_zhai  []={"Õ¬Õ­Õ®µÔÔñÕªÕ«Õ¯íÎñ©"};
const unsigned char PY_mb_zhan  []={"Õ¼Õ½Õ¾Õ¹Õ´ÕÀÕ°Õ±Õ²Õ³ÕµÕ¶Õ·Õ¸Õº²üÕ»Õ¿ÚŞŞøæöì¹"};
const unsigned char PY_mb_zhang []={"ÕÅÕÉÕÌÕÍÕÈÕÁÕÂÕÃÕÄÕÆÕÇÕÊÕË³¤ÕÎÕÏá¤áÖâ¯ó¯ØëÛµæÑè°"};
const unsigned char PY_mb_zhao  []={"ÕÒÕÙ×¦×ÅÕ×³¯ÕĞÕÑÚ¯ÕÓÕÔÕÕÕÖÕØîÈóÉßúèş"};
const unsigned char PY_mb_zhe   []={"ÕÚÕÛÕÜÕİÕŞÕßÕàÕáÕâÕã×ÅÚØß¡èÏéüíİğÑñŞòØô÷"};
const unsigned char PY_mb_zhen  []={"ÕæÕêÕëÕğÕñÕïÕóÕòÕìÕíÕäÕîÕåÕçóğÕéÖ¡ÛÚÕèİèä¥äÚçÇèåé©é»éôêâëÓëŞìõî³ğ¡ğ²"};
const unsigned char PY_mb_zheng []={"ÕıÖ¤ÕôÕõÕöÕ÷ÕøÕùÕúÕûÕüÕşÖ¢Ö£Úºá¿áçîÛï£óİöë"};
const unsigned char PY_mb_zhi   []={"ÖÊÖÃÖÆÖÎÖ§Ö»Ê¶Ö´ÖµÖ¹ÖÁÖ¥Ö®Ö­Ö¼Ö½Ö¾Ö¦ÖªÖ«Ö¬Ö¯Ö°Ö±Ö²Ö³Ö¶Ö·Ö¸ÖºÖ¿ÖÀÖÂÖÄÖÅÖÇÖÈÖÉÖ¨Ö©ÖËÖÌÖÍÖÏè×ÜÆØ´ÚìÛ¤ÛúŞıàùáçâºâååéåëæïèÎèÙèäéòéùêŞëÕëùìíìóíéïôğºğëòÎôêõ¥õÅõÙõÜõôö£"};
const unsigned char PY_mb_zhong []={"ÖĞÖÚÖÒÖÓÖÔÖÕÖÖÖ×ÖØÖÙÖÑÚ£âìïñó®ô±õà"};
const unsigned char PY_mb_zhou  []={"ÖÛÖÜÖİÖŞÖßÖàÖáÖâÖãÖäÖåÖæÖçÖèİ§ßúæ¨æûç§ëĞíØô¦ôíôü"};
const unsigned char PY_mb_zhu   []={"Ö÷×¡×¢×£ÖñÖéÖêÖëÖìÖíÖîÖïÖğÖòÖóÖôÖõÖöÖøÖùÖúÖûÖüÖıÖş×¤ØùóÃóçÙªÊôÊõÛ¥ÜÑÜïä¨ä¾äóèÌéÆéÍìÄîùğæğññÒô¶ôãõî÷æØ¼"};
const unsigned char PY_mb_zhua  []={"×¥×¦ÎÎ"};
const unsigned char PY_mb_zhuai []={"×§×ª"};
const unsigned char PY_mb_zhuan []={"´«×¨×©×ª×«×¬×­ßùâÍãçò§"};
const unsigned char PY_mb_zhuang[]={"´±×®×¯×°×±×²×³×´ŞÊí°"};
const unsigned char PY_mb_zhui  []={"×µ×¶×·×¸×¹×ºã·æíçÄö¿"};
const unsigned char PY_mb_zhun  []={"×»×¼â½ëÆñ¸"};
const unsigned char PY_mb_zhuo  []={"×Å×½×¾×¿×À×Æ×Â×Ç×Ã×Á×ÄÙ¾ÚÂßªä·äÃåªìÌìúí½ïí"};
const unsigned char PY_mb_zi    []={"×Ô×Ó×Ö×Ğ×È×É×Ê×Ë×Ì×Í×Î×Ï×Ñ×Ò×Õö·æ¢Ö¨áÑÚÑÜëæÜç»è÷ê¢êßí§íöïÅïöñèóÊôÒôôõşö¤öö÷Ú"};
const unsigned char PY_mb_zong  []={"×Ú×Û×Ü×××Ø×Ù×İôÕÙÌèÈëê"};
const unsigned char PY_mb_zou   []={"×ß×à×á×ŞÚÁÚîÛ¸æãöí"};
const unsigned char PY_mb_zu    []={"×ã×â×æ×ç×è×é×ä×åÙŞİÏïß"};
const unsigned char PY_mb_zuan  []={"×êß¬×ëçÚõò"};
const unsigned char PY_mb_zui   []={"×î×ï×ì×íŞ©õş"};
const unsigned char PY_mb_zun   []={"×ğ×ñé×÷®ß¤"};
const unsigned char PY_mb_zuo   []={"×ó×ô×ö×÷×ø×ù×ò´éßò×õÚè×ÁàÜâôëÑìñíÄõ¡"};

//Æ´ÒôË÷Òı±í
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
{"436","gen",sizeof(PY_mb_gan),(unsigned char*)PY_mb_gan},
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
{"4823","huai",sizeof(PY_mb_huai),(unsigned char*)PY_mb_huai},
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
