<tpl id="ui.topbar.tpl">
var menuList = f.config.menuList;
var userInfo = f.config.userInfo;
var pageInfo = f.config.pageInfo;
var systemConfig = f.config.systemConfig;
<nav class="ui-top-bar">
    <div class="wrapper">
        <ul class="title-area">
            <li class="name">
                if (menuList.title) {
                    <h1>
                        <a class="menu-toggle" href="#{menuList.title.url}">#{menuList.title.label}</a>
                    </h1>
                }
            </li>
        </ul>
        <section class="top-bar-section clearfix">
            <ul class="left">
                f.each(menuList.menus, function (menu) {
                    <li class="nav" data-menu-nav-id="#{menu.id}"><a class="menu-toggle" href="#{menu.url}">#{menu.label}</a></li>
                });
            </ul>
            <ul class="right notifications">
                if ('object' === typeof userInfo && !f.isEmpty(userInfo)) {
                    <li class="divider"></li>
                    <li class="has-dropdown">
                        <a href="javascript:void(0);" title="#{userInfo.user}" class="dropdown-a">#{f.prune(userInfo.user, 10)}<span class="dropdown-arrow">&nbsp;</span></a>
                        <ul class="dropdown">
                            f.each(menuList.userMenus, function (subMenu) {
                                <li class="dropdown-list" data-dropdown-menu-id="#{subMenu.id}">
                                    <a href="#{subMenu.url}" class="dropdown-menu">#{subMenu.label}</a>
                                </li>
                            });
                            if (menuList.userMenus.length) {
                                <li class="divider"></li>
                            }
                            <li class="dropdown-list">
                                <a href="#{this.logoutUrl}" class="dropdown-menu logout">退出登陆</a>
                            </li>
                        </ul>
                    </li>
                    <li class="divider"></li>
                    <li>
                        <a href="#{this.mailUrl}" title="查看信件" class="notification mail"><span class="notification-icon">&nbsp;</span><span class="notification-label">0</span></a>
                    </li>
                }
                else {
                    <li class="has-form">
                        <a href="javascript:void(0);" class="button login menu-toggle menu-toggle-new">登陆</a>
                    </li>
                }
            </ul>
        </section>
    </div>
</nav>
</tpl>
