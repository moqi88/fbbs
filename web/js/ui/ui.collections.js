/**
 * @fileOverview collection list
 * @author pzh
 */

(function ($, f) {
    'use strict';

    $.widget("ui.collections", {
        version: '1.0.0',
        options: {
            tpl: 'ui.sideBarList.tpl',
            collectionEditUrl: '',
            label: '我的收藏',
            editLabel: '编辑',
            emptyTip: '没有收藏版面',
            loadingTip: '载入中...',
            className: 'ui-collections-list',
            collectionUrl: '',
            onrefresh: null
        },
        render: function (list, option) {
            var options = $.extend({}, this.options, option);
            this.element.html(f.tpl.format(this.options.tpl, {
                className: options.className,
                label: options.label,
                rightLabel: options.editLabel,
                rightUrl: options.collectionEditUrl,
                emptyTip: options.emptyTip,
                loadingTip: options.loadingTip,
                loading: options.loading,
                list: list
            }));
        },
        empty: function () {
            this.render();
        },
        error: function (msg) {
            this.render(null, {
                emptyTip: msg
            });
        },
        loading: function () {
            this.render(null, {
                loadingTip: this.options.loadingTip,
                loading: true
            });
        },
        refresh: function () {
            var me = this;
            this.element.show();
            this.loading();
            $.jsonAjax('GET', this.options.collectionUrl, {}, function (data) {
                me.render(me._convertBoardData(data));
            }, function (msg) {
                me.error(msg);
            });
        },
        hide: function () {
            this.element.hide();
        },
        _convertBoardData: function (data) {
            var targetList = [];
            f.each(data, function (item) {
                targetList.push({
                    url: f.format(f.config.urlFormatter.board, {
                            bid: encodeURIComponent(item.bid)
                        }),
                    fullName: item.label,
                    name: item.board
                });
            });
            return targetList;
        }
    });
}(jQuery, f));
