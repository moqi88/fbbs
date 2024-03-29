#include <dlfcn.h>
#include "bbs.h"
#include "sysconf.h"
#include "fbbs/board.h"
#include "fbbs/mail.h"
#include "fbbs/session.h"
#include "fbbs/string.h"
#include "fbbs/terminal.h"

#ifndef DLM
#undef  ALLOWGAME
#endif

#ifdef FDQUAN
#define ALLOWGAME
#endif

int domenu(const char *menu_name);
int Announce(), Personal(), Info(), Goodbye();
int board_select(), Welcome();
int msg_more(), x_lockscreen();
int Conditions(), x_cloak(), show_online_users(), x_info(), x_vote();
int x_results(), ent_bnet(), a_edits(), x_edits();
int x_userdefine();
int m_new(), m_read(), m_send(), g_send();
int ov_send(), s_msg(), mailall(), offline();
//added by iamfat 2002.09.04
/*2003.04.23 added by stephen*/
int giveUpBBS();
/*stephen add end*/
extern int fill_reg_form(void);

int ent_bnet2();

#ifdef ALLOWGAME
int ent_winmine();
#endif

#ifdef INTERNET_EMAIL
int m_internet();
#endif

int show_online_followings(), t_list(), t_monitor();
int x_cloak();
//% int AddPCorpus(); // deardragon 个人文集 
int AddPCorpus(); // deardragon \xb8\xf6\xc8\xcb\xce\xc4\xbc\xaf 
extern int tui_following_list(void);
extern int tui_black_list(void);
extern int tui_query(void);

#ifndef WITHOUT_ADMIN_TOOLS
int m_vote();
#ifndef DLM
int x_new_denylevel();
int x_level(), m_info();
int m_register();
int d_board(), m_editbrd(), m_newbrd();
int setsystempasswd();
#endif
#endif

int wall();
static int exec_mbem(const char *s);

extern int tui_props(void);
extern int tui_my_props(void);

extern int tui_ordain_bm(const char *);
extern int tui_retire_bm(const char *);
extern int tui_new_board(const char *);
extern int tui_edit_board(const char *);

extern int post_list_reply(void);
extern int post_list_mention(void);

typedef int (*telnet_handler_t)();

typedef struct {
	const char *name;
	telnet_handler_t fptr;
} cmd_list_t;

#ifdef DLM
typedef struct {
	const char *name;
	const char *fptr;
} dlm_list_t;
#endif

static telnet_handler_t sysconf_funcptr(const char *name)
{
	static const cmd_list_t cmdlist[] = {
		{ "domenu", domenu },
		{ "EGroups", tui_read_sector },
		{ "BoardsAll", tui_all_boards },
		{ "BoardsGood", tui_favorite_boards },
		{ "BoardsNew", tui_unread_boards },
		{ "LeaveBBS", Goodbye },
		{ "Announce", Announce },
		{ "Personal", Personal },
		{ "SelectBoard", board_select },
		{ "ReadReply", post_list_reply },
		{ "ReadMention", post_list_mention },
		{ "MailAll", mailall },
		{ "LockScreen", x_lockscreen },
		{ "OffLine", offline },
		{ "GiveUpBBS", giveUpBBS },
		{ "ReadNewMail", m_new },
		{ "ReadMail", m_read },
		{ "SendMail", m_send },
		{ "GroupSend", g_send },
		{ "OverrideSend", ov_send },
#ifdef INTERNET_EMAIL
		{ "SendNetMail", m_internet },
#endif
		{ "UserDefine", x_userdefine },
		{ "ShowFriends", show_online_followings },
		{ "ShowLogins", show_online_users },
		{ "QueryUser", tui_query },
		{ "SetCloak", x_cloak },
		{ "SendMsg", s_msg },
		{ "ShowMsg", msg_more },
		{ "SetFriends", tui_following_list },
		{ "SetRejects", tui_black_list },
		{ "FillForm", fill_reg_form },
		{ "Information", x_info },
		{ "EditUFiles", x_edits },
		{ "ShowLicense", Conditions },
		{ "ShowVersion", Info },
		{ "Notepad", shownotepad },
		{ "Vote", x_vote },
		{ "VoteResult", x_results },
		{ "ExecBBSNet", ent_bnet },
		{ "ExecBBSNet2", ent_bnet2 },
		{ "ShowWelcome", Welcome },
		{ "AddPCorpus", AddPCorpus },
		{ "Props", tui_props },
		{ "MyProps", tui_my_props },
#ifdef ALLOWGAME
		{ "WinMine", ent_winmine },
#endif
		{ "RunMBEM", exec_mbem },
		{ "OpenVote", m_vote },
#ifndef DLM
		{ "Setsyspass", setsystempasswd },
		{ "Register", m_register },
		{ "ShowRegister", show_register },
		{ "Info", m_info },
		{ "Level", x_level },
		{ "OrdainBM", tui_ordain_bm },
		{ "RetireBM", tui_retire_bm },
		{ "NewChangeLevel", x_new_denylevel },
		{ "NewBoard", tui_new_board },
		{ "ChangeBrd", tui_edit_board },
		{ "BoardDel", d_board },
		{ "SysFiles", a_edits },
		{ "Wall", wall },
#endif
		{ NULL, NULL }
	};

	const cmd_list_t *cmd = cmdlist;
	while (cmd->name != NULL) {
		if (strcmp(name, cmd->name) == 0)
			return cmd->fptr;
		++cmd;
	}
	return NULL;
}

#ifdef DLM
static const char *sysconf_funcstr(const char *name)
{
	static const dlm_list_t dlmlist[] = {
#ifdef ALLOWGAME
		{ "Gagb", "@mod:so/game.so#gagb" },
		{ "BlackJack", "@mod:so/game.so#BlackJack" },
		{ "X_dice", "@mod:so/game.so#x_dice" },
		{ "P_gp", "@mod:so/game.so#p_gp" },
		{ "IP_nine", "@mod:so/game.so#p_nine" },
		{ "OBingo", "@mod:so/game.so#bingo" },
		{ "Chicken", "@mod:so/game.so#chicken_main" },
		{ "Mary", "@mod:so/game.so#mary_m" },
		{ "Borrow", "@mod:so/game.so#borrow" },
		{ "Payoff", "@mod:so/game.so#payoff" },
		{ "Impawn", "@mod:so/game.so#popshop" },
		{ "Doshopping", "@mod:so/game.so#doshopping" },
		{ "Lending", "@mod:so/game.so#lending" },
		{ "StarChicken", "@mod:so/pip.so#mod_default" },
#endif // ALLOWGAME
		{ "Kick", "@mod:so/admintool.so#kick_user" },
		{ "Setsyspass", "@mod:so/admintool.so#setsystempasswd" },
		{ "Register", "@mod:so/admintool.so#m_register" },
		{ "ShowRegister", "@mod:so/admintool.so#show_register" },
		{ "Info", "@mod:so/admintool.so#m_info" },
		{ "Level", "@mod:so/admintool.so#x_level" },
		{ "OrdainBM", "@mod:so/admintool.so#tui_ordain_bm" },
		{ "RetireBM", "@mod:so/admintool.so#tui_retire_bm" },
		{ "ChangeLevel", "@mod:so/admintool.so#x_denylevel" },
		{ "NewChangeLevel", "@mod:so/admintool.so#x_new_denylevel" },
		{ "NewBoard", "@mod:so/admintool.so#tui_new_board" },
		{ "ChangeBrd", "@mod:so/admintool.so#tui_edit_board" },
		{ "BoardDel", "@mod:so/admintool.so#d_board" },
		{ "SysFiles", "@mod:so/admintool.so#a_edits" },
		{ "Wall", "@mod:so/admintool.so#wall" },
		{ "TitleAdmin", "@mod:so/admintool.so#tui_title_list" },
		{ "MoneyAdmin", "@mod:so/admintool.so#grant_money" },
		{ "SearchAll", "@mod:so/admintool.so#tui_search_all_boards" },
		{ NULL, NULL }
	};

	const dlm_list_t *dlm = dlmlist;
	while (dlm->name != NULL) {
		if (strcmp(name, dlm->name) == 0)
			return dlm->fptr;
		++dlm;
	}
	return NULL;
}
#endif // DLM

/**
 * Execute function in dynamic loaded modules.
 * @param str function location, format: \@mod:[file]\#[function].
 * @return 0.
 */
static int exec_mbem(const char *str)
{
	char buf[128];
	strlcpy(buf, str, sizeof(buf));

	char *ptr = strstr(buf, "@mod:");
	if (ptr) {
		ptr = strstr(buf + 5, "#");
		if (ptr) {
			*ptr = '\0';
			++ptr;
		}

		void *hdll = dlopen(buf + 5, RTLD_LAZY);
		if (hdll) {
			int (*func)();
			*(void **)(&func) = dlsym(hdll, ptr ? ptr : "mod_main");
			if (func)
				func();
			dlclose(hdll);
		}
	}
	return 0;
}

static void decodestr(const char *str)
{
	register char ch;
	int n;

	while ((ch = *str++) != '\0')
		if (ch != '\01')
			outc(ch);
		else
			if (*str != '\0' && str[1] != '\0') {
				ch = *str++;
				n = *str++;
				while (--n >= 0)
					outc(ch);
			}
}

static int draw_menu(menuitem_t *pm)
{
	const char *str;

	screen_clear();
	int line = 3;
	int col = 0;
	int num = 0;

	while (1) {
		switch (pm->level) {
			case -1:
				return num;
			case -2:
				if (strcmp(pm->name, "title") == 0) {
					firsttitle(pm->desc);
				} else if (strcmp(pm->name, "screen") == 0) {
					if ((str = sysconf_str(pm->desc)) != NULL) {
						move(pm->line, pm->col);
						decodestr(str);
					}
				}
				break;
			default:
				if (pm->line >= 0 && HAS_PERM(pm->level)) {
					if (pm->line == 0) {
						pm->line = line;
						pm->col = col;
					} else {
						line = pm->line;
						col = pm->col;
					}
					move(line, col);
					prints("  %s", pm->desc);
					line++;
				} else {
					if (pm->line > 0) {
						line = pm->line;
						col = pm->col;
					}
					pm->line = -1;
				}
		}
		num++;
		pm++;
	}
}

extern void active_board_init(bool);
extern void active_board_show(void);

int domenu(const char *menu_name)
{
	extern int refscreen;
	int ch, i;

	if (sys_conf.items <= 0)
		return -1;

	menuitem_t *pm = sys_conf.item + sysconf_eval(menu_name, &sys_conf);

	int size = draw_menu(pm);

	int now = 0;

	user_id_t user_id = session_uid();
	// Jump to mail menu if user have unread mail.
	if (streq(menu_name, "TOPMENU") && (chkmail()
				|| post_reply_get_count(user_id)
				|| post_mention_get_count(user_id))) {
		for (i = 0; i < size; i++)
		if (pm[i].line> 0 && pm[i].name[0] == 'M')
		now = i;
	}

	set_user_status(ST_MMENU);
	active_board_show();

	while (1) {
		while (pm[now].level < 0 || !HAS_PERM(pm[now].level)) {
			now++;
			if (now >= size)
				now = 0;
		}

		move(pm[now].line, pm[now].col);
		prints(">");

		ch = egetch();

		move(pm[now].line, pm[now].col);
		prints(" ");

		switch (ch) {
			case EOF:
				// TODO: deprecate
				if (!refscreen) {
					abort_bbs(0);
				}
				draw_menu(pm);
				set_user_status(ST_MMENU);
				active_board_show();
				break;
			case KEY_RIGHT:
				for (i = 0; i < size; i++) {
					if (pm[i].line == pm[now].line && pm[i].level >= 0 &&
							pm[i].col > pm[now].col && HAS_PERM(pm[i].level))
						break;
				}
				// If there are items on the right to current item.
				if (i < size) {
					now = i;
					break;
				}
				// fall through.
			case '\n':
			case '\r':
				if (strcmp(pm[now].arg, "..") == 0)
					return 0;
				if (pm[now].func) {
					telnet_handler_t func = sysconf_funcptr(pm[now].func);
					if (func) {
						(*func)(pm[now].arg);
					} else {
#ifdef DLM
						const char *ptr = sysconf_funcstr(pm[now].func);
						if (!ptr)
							break;
						else
							exec_mbem(ptr);
#else
						;
#endif // DLM
					}
					draw_menu(pm);
					set_user_status(ST_MMENU);
					active_board_show();
				}
				break;
			case KEY_LEFT:
				for (i = 0; i < size; i++) {
					if (pm[i].line == pm[now].line && pm[i].level >= 0 &&
							pm[i].col < pm[now].col && HAS_PERM(pm[i].level))
						break;
					if (strcmp(pm[i].func, "LeaveBBS") == 0)
						break;
				}
				if (i < size) {
					now = i;
					break;
				}
				return 0;
			case KEY_DOWN:
				now++;
				break;
			case KEY_UP:
				now--;
				while (pm[now].level < 0 || !HAS_PERM(pm[now].level)) {
					if (now > 0)
						now--;
					else
						now = size - 1;
				}
				break;
			case KEY_PGUP:
				now = 0;
				break;
			case KEY_PGDN:
				now = size - 1;
				while (pm[now].level < 0 || !HAS_PERM(pm[now].level))
					now--;
				break;
			case '~':
				if (!(HAS_PERM(PERM_ESYSFILE)))
					break;
				sysconf_load(true);
				report("reload sysconf.img", currentuser.userid);
				pm = sys_conf.item + sysconf_eval(menu_name, &sys_conf);
				active_board_init(true);
				size = draw_menu(pm);
				now = 0;
				break;
			case '!':
				if (strcmp("TOPMENU", menu_name) == 0)
					break;
				else
					return 0;
			case Ctrl('T'):
				if (tui_check_notice(NULL) == FULLUPDATE) {
					draw_menu(pm);
					active_board_show();
				} else {
					tui_update_status_line();
				}
				break;
			default:
				if (ch >= 'a' && ch <= 'z')
					ch = ch - 'a' + 'A';
				for (i = 0; i < size; i++) {
					if (pm[i].line> 0 && ch == pm[i].name[0]
							&& HAS_PERM(pm[i].level)) {
						now = i;
						break;
				}
			}
		}
	}
}
