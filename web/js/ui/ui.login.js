(function ($, f) {
    'use strict';

    $.widget('ui.login', {
        options: {
            registUrl: 'javascript:void(0);',
            forgetUrl: 'javascript:void(0);',
            loginText: '登&emsp;&emsp;陆',
            dialogTitle: '登陆',
            loadingText: '登陆中...',
            trigger: '',
            overlayClass: 'ui-login-overlay',
            tpl: 'ui.login.tpl'
        },
        /**
         * widget 构造器
         *
         * @private
         */
        _create: function () {
            this.element.append(f.tpl.format(this.options.tpl, this.options));
            this._renderDialog();
            this._$id = this.element.find('.ui-login-input-id');
            this._$pw = this.element.find('.ui-login-input-pw');
            this._$rm = this.element.find('.ui-login-input-rm');
            this._$tip = this.element.find('.ui-login-tip');
            this._$inputs = this.element.find('.ui-login-input');
            this._$submit = this.element.find('.ui-login-submit');
            this._bindEvents();
        },
        _bindEvents: function () {
            $(this.options.trigger).on('click', $.proxy(this, 'open'));
            this._on({
                'click .ui-login-submit': '_onclickSubmit',
                'keydown .ui-login-input': '_onkeydownInput',
                'click .ui-login-label-tip': '_onclickTip'
            });
        },
        _renderDialog: function (options) {
            this.dialog = this.element;

            this.dialog.dialog({
                autoOpen: false,
                closeText: '关闭',
                dialogClass: 'ui-login-dialog',
                title: this.options.dialogTitle,
                modal: true,
                overlayClass: this.options.overlayClass,
                resizable: false,
                width: 270,
                maxHeight: 270
            });
        },
        _onclickSubmit: function (event) {
            event.preventDefault();
            if (this.validation()) {
                this._trigger('onsubmit', null, {
                    id: this._$id.val(),
                    pw: this._$pw.val(),
                    rm: this._$rm.prop('checked')
                });
            }
        },
        _onkeydownInput: function (event) {
            if (event.which === 13) {
                event.preventDefault();
                var $target = $(event.target);
                if ($target.attr('name') === 'pw') {
                    $target.blur();
                    this._onclickSubmit(event);
                }
                else {
                    var index = this._$inputs.index($target) + 1;
                    index === this._$inputs.length && (index = 0);
                    var $next = this._$inputs.eq(index);
                    if ($next.length) {
                        $next.focus();
                    }
                }
            }
        },
        _onclickTip: function () {
            this.close();
        },
        open: function () {
            this.dialog.dialog('open');
        },
        close: function () {
            this.dialog.dialog('close');
        },
        error: function (err) {
            this._$tip.text(err);
        },
        validation: function () {
            this.error('');
            var err = '';
            if (this._$id.val() === '') {
                err = '请输入账号';
                this._$id.focus();
            }
            else if (!/[a-z]{2,12}/i.test(this._$id.val())) {
                err = '请输入合法账号';
                this._$id.focus();
            }
            else if (this._$pw.val() === '') {
                err = '请输入密码';
                this._$pw.focus();
            }
            else {
                return true;
            }
            this.error(err);
            return false;
        },
        loading: function () {
            this._$submit
                .prop('disabled', true)
                .addClass('disabled')
                .html(this.options.loadingText);
            this._$inputs
                .prop('disabled', true)
                .addClass('disabled');
            this.element.addClass('loading');
        },
        reset: function () {
            this._$submit
                .prop('disabled', false)
                .removeClass('disabled')
                .html(this.options.loginText);
            this._$inputs
                .prop('disabled', false)
                .removeClass('disabled');
            this.element.removeClass('loading');
        }
    });
}(jQuery, f));