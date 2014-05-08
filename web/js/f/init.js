/**
 * @fileOverview Initializing system, and this is the entry of system
 * @author pzh
 */

(function ($, f) {
    'use strict';

    $.extend(f, (function () {
        /**
         * Initializing widget of system and regiser global event callback handler
         */

        function initBBS() {
            // Init platform
            $.each($.platform, function (key, value) {
                if (value) {
                    $('html').addClass(key);
                }
            });

            // Get user's cookies
            var user = {
                user: $.cookie('utmpuserid'),
                token: $.cookie('utmpkey')
            };
            if (user.user && user.token) {
                $.extend(f.config.userInfo, user);
            }

            // Render topbar
            $('.menu-nav').topbar({
                onchange: function (event, data) {
                    f.mediator.trigger('app:topbar:onchange', data);
                },
                urls: {
                    userInfoUrl: f.config.urlConfig.userInfo,
                    logoutUrl: f.config.urlConfig.logout,
                    loginUrl: f.config.urlConfig.login,
                    mailUrl: f.config.urlConfig.mail
                }
            });

            // Render footer
            $('#footer').html(f.tpl.format('ui.footer.tpl', {
                copyright: f.config.systemConfig.copyright,
                menu: f.config.menuList.footerMenus,
                icp: f.config.systemConfig.icp
            }));

            // Show loading
            $(document.body).loading();

            // history back click event handler
            $('.body').on('click', '.history-back', function () {
                window.history.back();
            });

            // hide layer click event handler
            $('.body').on('click', '.cancel-layer', function () {
                $(document).trigger('click');

                return false;
            });

            // Enable notifications
            $('.notifications').on('click', function () {
                if (window.webkitNotifications && window.webkitNotifications.checkPermission() == 1) {
                    window.webkitNotifications.requestPermission();
                }
            });

        }

        /**
         * Load template from server, then parse and cache the template on the client-side
         */

        function loadTemplate() {
            $.ajax({
                type: 'GET',
                url: f.config.systemConfig.webRoot + '/tpl/tpl.html??__v=@version@',
                async: false
            }).done(function (data, textStatus, jqXHR) {
                parseTemplate(String(data));
            }).fail(function () {});
        }

        /**
         * Parse and cache the client-side template
         *
         * @param  {string} tplText Template text value
         */

        function parseTemplate(tplText) {
            var tplSettings = [];
            var pattern = /<(script|tpl)[^>]*id=['"]([\w-\.]*)['"][^>]*>([\s\S]*?)<\/\1>/g;
            var result;

            while ((result = pattern.exec(tplText)) != null) {
                tplSettings.push({
                    id: result[2],
                    tpl: result[3]
                });
            }

            var i = tplSettings.length;

            while (i--) {
                var tpl = tplSettings[i];

                f.tpl.register(tpl.id, tpl.tpl);
            }
        }

        /**
         * The entry of system, initializing template, system and register hash change event etc.
         */

        function init() {
            loadTemplate();
            initBBS();

            // Load saved page
            var defaultPage = f.config.systemConfig.defaultPage;
            if (window.localStorage) {
                var idleTime = parseInt(window.localStorage.getItem('idleTime'));
                var time = (new Date()).getTime();
                if (time - idleTime <= f.config.systemConfig.idleTime) {
                    defaultPage = window.localStorage.getItem('idlePage') || defaultPage;
                }
            }
            $('#body').opoa({
                baseUri: f.config.systemConfig.baseUri,
                defaultPage: defaultPage,
                beforeClose: function () {
                    $(document.body).loading('show');
                },
                onclose: function () {
                    if (f.bbsPage) {
                        f.bbsPage.close();
                    }
                },
                onfail: function () {
                    document.title = f.config.systemConfig.defaultTitle;
                    $(document.body).loading('hide');
                    $('#main').html('<p class="loading-error">载入失败，请刷新重试。</p>');
                },
                afterClose: function (event, data) {
                    var url = data.url;
                    var fullHash = data.fullHash;
                    var pageInfo = f.config.pageInfo;
                    var menuNav = $(':data(ui-topbar)');
                    var globalTip = $('globalTip');
                    var body = $('#body');
                    var title = pageInfo.title || f.config.systemConfig.defaultTitle;

                    // Set title
                    document.title = title;

                    // Hide loading
                    $(document.body).loading('hide');

                    // Close all tips
                    globalTip.tip('close');

                    // Reset notifications
                    $(document.body).notifications('reset');

                    // Change login url
                    if (f.config.pageInfo.pageName !== 'login') {
                        menuNav.topbar('setUrl', {
                            selector: '.menu-toggle-login',
                            url: [f.config.urlConfig.login, '?r=', encodeURIComponent(fullHash)].join("")
                        });
                    }

                    // Set active menu nav
                    if (pageInfo.menuNavId) {
                        menuNav.topbar('setActivate', pageInfo.menuNavId);
                    }

                    // Toggle side nav
                    body.toggleClass('hide-side-nav', pageInfo.showSideNav === false);

                    // ie6,7提示
                    if (($.browser.msie && parseInt($.browser.version) < 8) || ($.browser.webkit && parseInt($.browser.version) < 5) || ($.browser.opera && parseInt($.browser.version) < 11) || ($.browser.mozilla && parseInt($.browser.version) < 4)) {
                        globalTip.tip('append', {
                            id: 'browser-alert',
                            boxClass: 'warning',
                            content: f.format('提示：本系统不支持IE6、IE7浏览器！请升级浏览器。<a target="_target" href="!{firefox}">Firefox浏览器下载</a>&#12288;|&#12288;<a target="_target" href="!{chrome}">Chrome浏览器下载</a>', {
                                chrome: 'http://www.google.cn/intl/zh-CN/chrome/browser/',
                                firefox: 'http://firefox.com.cn/download/'
                            })
                        });
                    }

                    // Save idle status
                    if (window.localStorage) {
                        window.localStorage.setItem('idlePage', fullHash);
                        window.localStorage.setItem('idleTime', (new Date()).getTime());
                    }
                }
            });
        }

        return {
            init: init,
            bbsPage: null
        };

    })());
}(jQuery, f));
