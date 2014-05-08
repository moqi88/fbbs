/**
 * @fileOverview Initializing components
 * @author pzh
 */

f.namespace('f.components');

(function ($, f) {
    /**
     * 获取事件名
     *
     * @private
     * @param {string} name 事件名称
     * @param {string} prefix 事件名称前缀
     *
     * @return {string} 拼装后的事件名称
     */
    var _getChannel = function (name, prefix) {
        return prefix ? prefix + name : "onchange" + name;
    };

    f.extend(f.components, {
        Ajax: function (thisValue) {
            f.extend(thisValue, {
                /**
                 * 封装ajax操作，添加回调事件支持
                 *
                 * @public
                 * @param  {Object} method   方法对象
                 * @config {Object} postData 发送请求时的数据
                 * @config {*}      extData  根据业务需求，发送请求时自行添加的额外数据
                 */
                ajax: function (method, postData, extData) {
                    var type = method.type || 'GET';
                    var url = this.systemConfig.ajaxUri + '/' + method.url;
                    postData = postData || {};

                    //触发正在请求中事件……
                    this.trigger(_getChannel(method.name, 'on'), {
                        postData: postData,
                        extData: extData
                    });

                    $.jsonAjax(type, url, postData, function (data, status, msg) {
                        // whether the data adpter exist?
                        if (f.isString(method.adapter) && f.isFunction(thisValue.DataAdapter[method.adapter])) {
                            data.status = status;
                            thisValue.trigger(_getChannel(f.capitalize(method.adapter), 'before'), data);
                            data = thisValue.DataAdapter[method.adapter](data, thisValue);
                            thisValue.trigger(_getChannel(f.capitalize(method.adapter), 'after'), data);
                        }

                        thisValue.trigger(_getChannel(method.name + 'Success', 'on'), data, {
                            status: status,
                            postData: postData,
                            extData: extData,
                            msg: msg
                        });
                    }, function (msg, data) {
                        thisValue.trigger(_getChannel(method.name + 'Failed', "on"), msg, {
                            data: data,
                            postData: postData,
                            extData: extData
                        });
                    });
                }
            });
        },
        /*
        Breadcrumb: function (thisValue) {
            f.extend(thisValue, {
                _initBreadcrumb: function (params) {
                    this[params.id] = $('#breadcrumb').breadcrumb({
                        items: thisValue.breadcrumbList
                    }).breadcrumb('instance');
                }
            });
        },
        ToggleTarget: function (thisValue) {
            $('.toggleable').toggleTarget({
                onchange: function (event, data) {
                    thisValue.trigger(_getChannel('ToggleTarget'), data);
                    $(window).trigger('resize');
                }
            });
        },
        ScrollTarget: function (thisValue) {
            $('.scroll').scrollTarget();
        },
        */
        DataAdapter: function (thisValue) {
            thisValue['DataAdapter'] = new f.dataAdapter();
        },
        Tip: function (thisValue) {
            thisValue['Tip'] = $('#global-tip').tip();
        },
        Notifications: function (thisValue) {
            thisValue['Notifications'] = $(document.body).notifications({
                notificationUrl: [f.config.systemConfig.ajaxUri, '/user/notifications.json'].join(""),
                getInterval: f.config.systemConfig.notificationInterval,
                // Check if logged in
                ongetNotification: function () {
                    $('.left-off-canvas-toggle i').removeClass('notified');
                    return !!(f.config.userInfo.user && f.config.userInfo.token);
                },
                // Get notifications
                ongetNotificationSuccess: function (event, data) {
                    var notified = [],
                        newMsg = false,
                        list = {
                            mail: '您有#{0}封未读信件。'
                        };
                    f.each(data, function (value, key) {
                        var noti = $('.notifications').find(['.', key].join("")),
                            icon = noti.find('.notification-icon'),
                            label = noti.find('.notification-label'),
                            text = label.text();
                        // If there are new notifications
                        if (value) {
                            newMsg = true;
                            // Set url when get new messages
                            noti.addClass('notified')
                                .attr('href', f.config.urlConfig[['new', f.capitalize(key)].join("")]);
                            // Show number of messages
                            label.text(value);
                            // If more new messages
                            if (~~text < value) {
                                // Save notification text
                                notified.push(f.format(list[key], value));
                                notified.push('\n');
                            }
                            // Show animation
                            icon.addClass('tada');
                            setTimeout(function () {
                                icon.removeClass('tada');
                            }, 1000);
                        }
                        else {
                            noti.removeClass('notified')
                                .attr('href', f.config.urlConfig[key]);
                            label.text(value);
                        }
                    });

                    // Set small notification dot
                    var smallIcon = $('.left-off-canvas-toggle i');
                    if (newMsg) {
                        if (smallIcon.hasClass('notified')) {
                            smallIcon.addClass('pulse');
                            setTimeout(function () {
                                smallIcon.removeClass('pulse');
                            }, 1000);
                        }
                        smallIcon.addClass('notified');
                    }
                    else {
                        smallIcon.removeClass('notified');
                    }

                    // Check hidden prop of window
                    var getHiddenProp = function () {
                        var prefixes = ['webkit','moz','ms','o'];

                        // if 'hidden' is natively supported just return it
                        if ('hidden' in document) {
                            return 'hidden';
                        }

                        // otherwise loop over all the known prefixes until we find one
                        for (var i = 0; i < prefixes.length; i++) {
                            if ((prefixes[i] + 'Hidden') in document) {
                                return prefixes[i] + 'Hidden';
                            }
                        }

                        // otherwise it's not supported
                        return null;
                    };

                    // Return if the window is hidden
                    var isHidden = function () {
                        var prop = getHiddenProp();
                        if (!prop) {
                            return false;
                        }

                        return document[prop];
                    };

                    // If the window is hidden and there are new notifications, display them
                    if (isHidden() && notified.length) {
                        if (window.webkitNotifications && window.webkitNotifications.checkPermission() == 0) {
                            var notification = webkitNotifications.createNotification(
                                [f.config.systemConfig.webRoot, '/images/apple-touch-icon-iphone.png'].join(""),
                                f.config.systemConfig.defaultTitle,
                                notified.join("")
                            );
                            notification.onclick = function() {
                                this.cancel();
                            };
                            notification.replaceId = 'bbsNotification';
                            notification.show();
                        }
                    }
                }
            });
        },
        BoardSearch: function (thisValue) {
            f.extend(thisValue, {
                _initBoardSearch: function (params) {
                    var me = this;
                    this[params.id] = $('.board-search-container').boardSearch(f.extend({
                        searchDelay: 700,
                        searchUrl: this.systemConfig.ajaxUri + '/boards/query.json',
                        onsearch: function (data) {
                            //me.Collections.hide();
                        },
                        onclickTarget: function (event, data) {
                            $('.exit-canvas-menu').trigger('mousedown');
                            data.target && (window.location.href = data.url);
                        }
                    }, params.options));
                }
            });
        },
        Login: function (thisValue) {
            f.extend(thisValue, {
                _initLogin: function (params) {
                    var me = this;
                    var selector = params.selector || '#login-channel';
                    this[params.id] = $(selector).login(f.extend({
                        trigger: '.login',
                        registUrl: f.config.urlConfig.regist,
                        forgetUrl: f.config.urlConfig.forget,
                        cookiePath: f.config.systemConfig.cookiePath,
                        loginUrl: f.config.systemConfig.ajaxUri + '/user/login.json',
                        onloginSuccess: function (event, data) {

                            // Copy userInfo
                            f.config.userInfo = f.clone(data);

                            // Rerender the topbar
                            $('.menu-nav').topbar('render');

                            // Check notifications
                            $(document.body).notifications('reset');

                            // Render collections
                            me.Collections.refresh();

                            // Jump to last page
                            if (f.config.pageInfo.params.r) {
                                window.location.href = f.config.pageInfo.params.r;
                            }
                            else if (f.config.pageInfo.pageName === 'login') {
                                window.location.href = f.config.systemConfig.defaultPage;
                            }
                        }
                    }, params.options));
                }
            });
        },
        Collections: function (thisValue) {
            f.extend(thisValue, {
                _initCollections: function (params) {
                    this[params.id] = $('.collections-container').collections(f.extend({
                        collectionEditUrl: f.config.urlConfig.collectionEdit,
                        collectionUrl: f.config.systemConfig.ajaxUri + '/collections/list.json'
                    }, params.options)).collections('instance');
                    if (f.config.userInfo.user && f.config.userInfo.token) {
                        $('.collections-container').collections('refresh');
                    }
                }
            });
        }

    });

    f.extend(f.components, {

    });
}(jQuery, f));
