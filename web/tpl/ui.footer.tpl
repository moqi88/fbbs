<tpl id="ui.footer.tpl">
    <div class="footer-content wrapper clearfix">
        <div class="footer-right right">
            <ul class="footer-menu clearfix">
                f.each(this.menu, function (nav) {
                    <li class="footer-list footer-nav">
                        <a href="#{nav.url}" target="#{nav.target ? '_blank' : '_self'}">#{nav.label}</a>
                    </li>
                });
                <li class="footer-list footer-icp">
                    <a href="#{this.icp.url}" target="_blank">#{this.icp.label}</a>
                </li>
            </ul>
        </div>
        <div class="footer-left left">!#{this.copyright}</div>
    </div>
</tpl>
