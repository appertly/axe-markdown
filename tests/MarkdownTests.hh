<?hh

use HackPack\HackUnit\Contract\Assert;
use League\CommonMark\DocParser;
use League\CommonMark\Environment;

class MarkdownTest
{
    private ?DocParser $docParser;

    <<Setup>>
    public function setUp(): void
    {
        $environment = Environment::createCommonMarkEnvironment();
        $this->docParser = new DocParser($environment);
    }

    <<Test>>
    public async function testNoParser(Assert $assert): Awaitable<void>
    {
        $in = "# Hello\n\nThis is great.\n\n* I hope you like it";
        $out = '<h1>Hello</h1><p>This is great.</p><ul><li>I hope you like it</li></ul>';
        $a = <axe:markdown text={$in}/>;
        $assert->string((string)$a)->is($out);
    }

    <<Test>>
    public async function testTicks(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '`foo`' => '<p><code>foo</code></p>',
            '`` foo ` bar  ``' => '<p><code>foo ` bar</code></p>',
            '` `` `' => '<p><code>``</code></p>',
            "``\nfoo\n``" => '<p><code>foo</code></p>',
            "`foo   bar\n  baz`" => '<p><code>foo bar baz</code></p>',
            '`foo `` bar`' => '<p><code>foo `` bar</code></p>',
            '`foo\`bar`' => '<p><code>foo\</code>bar`</p>',
            '*foo`*`' => '<p>*foo<code>*</code></p>',
            '[not a `link](/foo`)' => '<p>[not a <code>link](/foo</code>)</p>',
            '`<a href="`">`' => '<p><code>&lt;a href=&quot;</code>&quot;&gt;`</p>',
            '<http://foo.bar.`baz>`' => '<p><a href="http://foo.bar.%60baz">http://foo.bar.`baz</a>`</p>',
            '```foo``' => '<p>```foo``</p>',
            '`foo' => '<p>`foo</p>',
            '`<http://foo.bar.`baz>`' => '<p><code>&lt;http://foo.bar.</code>baz&gt;`</p>',
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testAutolinks(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '<http://foo.bar.baz>' => '<p><a href="http://foo.bar.baz">http://foo.bar.baz</a></p>',
            '<http://foo.bar.baz/test?q=hello&id=22&boolean>' => '<p><a href="http://foo.bar.baz/test?q=hello&amp;id=22&amp;boolean">http://foo.bar.baz/test?q=hello&amp;id=22&amp;boolean</a></p>',
            '<irc://foo.bar:2233/baz>' => '<p><a href="irc://foo.bar:2233/baz">irc://foo.bar:2233/baz</a></p>',
            '<MAILTO:FOO@BAR.BAZ>' => '<p><a href="MAILTO:FOO@BAR.BAZ">MAILTO:FOO@BAR.BAZ</a></p>',
            '<a+b+c:d>' => '<p><a href="a+b+c:d">a+b+c:d</a></p>',
            '<made-up-scheme://foo,bar>' => '<p><a href="made-up-scheme://foo,bar">made-up-scheme://foo,bar</a></p>',
            '<http://../>' => '<p><a href="http://../">http://../</a></p>',
            '<localhost:5001/foo>' => '<p><a href="localhost:5001/foo">localhost:5001/foo</a></p>',
            '<http://foo.bar/baz bim>' => '<p>&lt;http://foo.bar/baz bim&gt;</p>',
            '<http://example.com/\[\>' => '<p><a href="http://example.com/%5C%5B%5C">http://example.com/\[\</a></p>',
            '<foo@bar.example.com>' => '<p><a href="mailto:foo@bar.example.com">foo@bar.example.com</a></p>',
            '<foo+special@Bar.baz-bar0.com>' => '<p><a href="mailto:foo+special@Bar.baz-bar0.com">foo+special@Bar.baz-bar0.com</a></p>',
            '<>' => '<p>&lt;&gt;</p>',
            '< http://foo.bar >' => '<p>&lt; http://foo.bar &gt;</p>',
            '<m:abc>' => '<p>&lt;m:abc&gt;</p>',
            '<foo.bar.baz>' => '<p>&lt;foo.bar.baz&gt;</p>',
            'http://example.com' => '<p>http://example.com</p>',
            'foo@bar.example.com' => '<p>foo@bar.example.com</p>',
            '<foo\+@bar.example.com>' => '<p>&lt;foo+@bar.example.com&gt;</p>',
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testEmphasisRule1(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '*test*' => '<p><em>test</em></p>',
            'foo *test* bar' => '<p>foo <em>test</em> bar</p>',
            '*foo* *test* *bar*' => '<p><em>foo</em> <em>test</em> <em>bar</em></p>',
            // CommonMark
            '*foo bar*' => '<p><em>foo bar</em></p>',
            'a * foo bar*' => '<p>a * foo bar*</p>',
            'a*"foo"*' => '<p>a*&quot;foo&quot;*</p>',
            ' * a *' => '<p> * a *</p>',
            '5*6*78' => '<p>5<em>6</em>78</p>',
            'foo*bar*' => '<p>foo<em>bar</em></p>',
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testEmphasisRule2(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '_test_' => '<p><em>test</em></p>',
            'foo _test_ bar' => '<p>foo <em>test</em> bar</p>',
            '_foo_ _test_ _bar_' => '<p><em>foo</em> <em>test</em> <em>bar</em></p>',
            'not_applicable_here but _this_ is' => '<p>not_applicable_here but <em>this</em> is</p>',
            // CommonMark
            '_foo bar_' => '<p><em>foo bar</em></p>',
            '_ foo bar_' => '<p>_ foo bar_</p>',
            'a_"foo"_' => '<p>a_&quot;foo&quot;_</p>',
            'foo_bar_' => '<p>foo_bar_</p>',
            '5_6_78' => '<p>5_6_78</p>',
            'пристаням_стремятся_' => '<p>пристаням_стремятся_</p>',
            'aa_"bb"_cc' => '<p>aa_&quot;bb&quot;_cc</p>',
            'foo-_(bar)_' => '<p>foo-<em>(bar)</em></p>'
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testEmphasisRule3(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '_foo*' => '<p>_foo*</p>',
            '*foo bar *' => '<p>*foo bar *</p>',
            '*(*foo)' => '<p>*(*foo)</p>',
            '*(*foo*)*' => '<p><em>(<em>foo</em>)</em></p>',
            '*foo*bar' => '<p><em>foo</em>bar</p>'
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testEmphasisRule4(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '_foo bar _' => '<p>_foo bar _</p>',
            '_(_foo)' => '<p>_(_foo)</p>',
            '_(_foo_)_' => '<p><em>(<em>foo</em>)</em></p>',
            '_foo_bar' => '<p>_foo_bar</p>',
            '_пристаням_стремятся' => '<p>_пристаням_стремятся</p>',
            '_foo_bar_baz_' => '<p><em>foo_bar_baz</em></p>',
            '_(bar)_.' => '<p><em>(bar)</em>.</p>'
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testLinks(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '[link](/uri "title")' => '<p><a href="/uri" title="title">link</a></p>',
            '[link](/uri)' => '<p><a href="/uri">link</a></p>',
            '[link]()' => '<p><a href="">link</a></p>',
            '[link](<>)' => '<p><a href="">link</a></p>',
            '[link](/my uri)' => '<p>[link](/my uri)</p>',
            '[link](</my uri>)' => '<p>[link](&lt;/my uri&gt;)</p>',
            "[link](foo\nbar)" => "<p>[link](foo\nbar)</p>",
            "[link](<foo\nbar>)" => "<p>[link](<foo\nbar>)</p>",
            '[link](\(foo\))' => '<p><a href="(foo)">link</a></p>',
            '[link]((foo)and(bar))' => '<p><a href="(foo)and(bar)">link</a></p>',
            '[link](foo(and(bar)))' => '<p>[link](foo(and(bar)))</p>',
            '[link](foo(and\(bar\)))' => '<p><a href="foo(and(bar))">link</a></p>',
            '[link](<foo(and(bar))>)' => '<p><a href="foo(and(bar))">link</a></p>',
            '[link](foo\)\:)' => '<p><a href="foo):">link</a></p>',
            "[link](#fragment)\n\n[link](http://example.com#fragment)\n\n[link](http://example.com?foo=3#frag)" => '<p><a href="#fragment">link</a></p><p><a href="http://example.com#fragment">link</a></p><p><a href="http://example.com?foo=3#frag">link</a></p>',
            '[link](foo\bar)' => '<p><a href="foo%5Cbar">link</a></p>',
            '[link](foo%20b&auml;)' => '<p><a href="foo%20b%C3%A4">link</a></p>',
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testLinkQuotesSpace(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '[link]("title")' => '<p><a href="%22title%22">link</a></p>',
            "[link](/url \"title\")\n[link](/url 'title')\n[link](/url (title))" => '<p><a href="/url" title="title">link</a>' . "\n" . '<a href="/url" title="title">link</a>' . "\n" . '<a href="/url" title="title">link</a></p>',
            '[link](/url "title \"&quot;")' => '<p><a href="/url" title="title &quot;&quot;">link</a></p>',
            '[link](/url "title "and" title")' => '<p>[link](/url &quot;title &quot;and&quot; title&quot;)</p>',
            '[link](/url \'title "and" title\')' => '<p><a href="/url" title="title &quot;and&quot; title">link</a></p>',
            "[link](   /uri\n  \"title\"  )" => '<p><a href="/uri" title="title">link</a></p>',
            '[link] (/uri)' => '<p>[link] (/uri)</p>',
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testLinkBraces(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '[link [foo [bar]]](/uri)' => '<p><a href="/uri">link [foo [bar]]</a></p>',
            '[link] bar](/uri)' => '<p>[link] bar](/uri)</p>',
            '[link [bar](/uri)' => '<p>[link <a href="/uri">bar</a></p>',
            '[link \[bar](/uri)' => '<p><a href="/uri">link [bar</a></p>',
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testLinkInline(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '[link *foo **bar** `#`*](/uri)' => '<p><a href="/uri">link <em>foo <strong>bar</strong> <code>#</code></em></a></p>',
            '[![moon](moon.jpg)](/uri)' => '<p><a href="/uri"><img src="moon.jpg" alt="moon"></a></p>'
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testLinkPrecedence(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            '[foo [bar](/uri)](/uri)' => '<p>[foo <a href="/uri">bar</a>](/uri)</p>',
            '[foo *[bar [baz](/uri)](/uri)*](/uri)' => '<p>[foo <em>[bar <a href="/uri">baz</a>](/uri)</em>](/uri)</p>',
            '![[[foo](uri1)](uri2)](uri3)' => '<p><img src="uri3" alt="[foo](uri2)"></p>',
            '*[foo*](/uri)' => '<p>*<a href="/uri">foo*</a></p>',
            '[foo *bar](baz*)' => '<p><a href="baz*">foo *bar</a></p>',
            '*foo [bar* baz]' => '<p><em>foo [bar</em> baz]</p>',
            '[foo <bar attr="](baz)">' => '<p>[foo <bar attr="](baz)"></p>',
            '[foo`](/uri)`' => '<p>[foo<code>](/uri)</code></p>',
            '[foo<http://example.com/?search=](uri)>' => '<p>[foo<a href="http://example.com/?search=%5D(uri)">http://example.com/?search=](uri)</a></p>',
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testLinkReference(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "[foo][bar]\n\n[bar]: /url \"title\"" => '<p><a href="/url" title="title">foo</a></p>',
            "[link [foo [bar]]][ref]\n\n[ref]: /uri" => '<p><a href="/uri">link [foo [bar]]</a></p>',
            "[link \[bar][ref]\n\n[ref]: /uri" => '<p><a href="/uri">link [bar</a></p>',
            "[link *foo **bar** `#`*][ref]\n\n[ref]: /uri" => '<p><a href="/uri">link <em>foo <strong>bar</strong> <code>#</code></em></a></p>',
            "[![moon](moon.jpg)][ref]\n\n[ref]: /uri" => '<p><a href="/uri"><img src="moon.jpg" alt="moon"></a></p>',
            "[foo [bar](/uri)][ref]\n\n[ref]: /uri" => '<p>[foo <a href="/uri">bar</a>]<a href="/uri">ref</a></p>',
            "[foo *bar [baz][ref]*][ref]\n\n[ref]: /uri" => '<p>[foo <em>bar <a href="/uri">baz</a></em>]<a href="/uri">ref</a></p>',
            "*[foo*][ref]\n\n[ref]: /uri" => '<p>*<a href="/uri">foo*</a></p>',
            "[foo *bar][ref]\n\n[ref]: /uri" => '<p><a href="/uri">foo *bar</a></p>',
            "[foo <bar attr=\"][ref]\">\n\n[ref]: /uri" => '<p>[foo <bar attr="][ref]"></p>',
            "[foo`][ref]`\n\n[ref]: /uri" => '<p>[foo<code>][ref]</code></p>',
            "[foo<http://example.com/?search=][ref]>\n\n[ref]: /uri" => '<p>[foo<a href="http://example.com/?search=%5D%5Bref%5D">http://example.com/?search=][ref]</a></p>',
            "[foo][BaR]\n\n[bar]: /url \"title\"" => '<p><a href="/url" title="title">foo</a></p>',
            "[Толпой][Толпой] is a Russian word.\n\n[ТОЛПОЙ]: /url" => '<p><a href="/url">Толпой</a> is a Russian word.</p>',
            "[Foo\n  bar]: /url\n\n[Baz][Foo bar]" => '<p><a href="/url">Baz</a></p>',
            "[foo] [bar]\n\n[bar]: /url \"title\"" => '<p>[foo] <a href="/url" title="title">bar</a></p>',
            "[foo]\n[bar]\n\n[bar]: /url \"title\"" => "<p>[foo]\n<a href=\"/url\" title=\"title\">bar</a></p>",
            "[foo]: /url1\n\n[foo]: /url2\n\n[bar][foo]" => '<p><a href="/url1">bar</a></p>',
            "[bar][foo\!]\n\n[foo!]: /url" => '<p>[bar][foo!]</p>',
            "[foo][ref[]\n\n[ref[]: /uri" => '<p>[foo][ref[]</p><p>[ref[]: /uri</p>',
            "[foo][ref[bar]]\n\n[ref[bar]]: /uri" => '<p>[foo][ref[bar]]</p><p>[ref[bar]]: /uri</p>',
            "[[[foo]]]\n\n[[[foo]]]: /url" => '<p>[[[foo]]]</p><p>[[[foo]]]: /url</p>',
            "[foo][ref\[]\n\n[ref\[]: /uri" => '<p><a href="/uri">foo</a></p>',
            "[bar\\\\]: /uri\n\n[bar\\\\]" => '<p><a href="/uri">bar\</a></p>',
            "[]\n\n[]: /uri" => '<p>[]</p><p>[]: /uri</p>',
            "[\n ]\n\n[\n ]: /uri" => "<p>[\n]</p><p>[\n]: /uri</p>"
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testLinkReferenceCollapsed(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "[foo][]\n\n[foo]: /url \"title\"" => '<p><a href="/url" title="title">foo</a></p>',
            "[*foo* bar][]\n\n[*foo* bar]: /url \"title\"" => '<p><a href="/url" title="title"><em>foo</em> bar</a></p>',
            "[Foo][]\n\n[foo]: /url \"title\"" => '<p><a href="/url" title="title">Foo</a></p>',
            "[foo] \n[]\n\n[foo]: /url \"title\"" => '<p><a href="/url" title="title">foo</a>' . "\n" . '[]</p>'
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testAtxHeadings(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "# foo\n## foo\n### foo\n#### foo\n##### foo\n###### foo" => '<h1>foo</h1><h2>foo</h2><h3>foo</h3><h4>foo</h4><h5>foo</h5><h6>foo</h6>',
            '####### foo' => '<p>####### foo</p>',
            "#5 bolt\n\n#hashtag" => '<p>#5 bolt</p><p>#hashtag</p>',
            "#\tfoo" => "<p>#\tfoo</p>",
            '\## foo' => '<p>## foo</p>',
            '# foo *bar* \*baz\*' => '<h1>foo <em>bar</em> *baz*</h1>',
            '#                  foo                     ' => '<h1>foo</h1>',
            " ### foo\n  ## foo\n   # foo" => '<h3>foo</h3><h2>foo</h2><h1>foo</h1>',
            '    # foo' => '<pre><code># foo' . "\n" . '</code></pre>',
            "foo\n    # bar" => "<p>foo\n# bar</p>",
            "## foo ##\n  ###   bar    ###" => '<h2>foo</h2><h3>bar</h3>',
            "# foo ##################################\n##### foo ##" => '<h1>foo</h1><h5>foo</h5>',
            '### foo ###     ' => '<h3>foo</h3>',
            '### foo ### b' => '<h3>foo ### b</h3>',
            '# foo#' => '<h1>foo#</h1>',
            "### foo \###\n## foo #\##\n# foo \#" => '<h3>foo ###</h3><h2>foo ###</h2><h1>foo #</h1>',
            "****\n## foo\n****" => '<hr><h2>foo</h2><hr>',
            "Foo bar\n# baz\nBar foo" => '<p>Foo bar</p><h1>baz</h1><p>Bar foo</p>',
            "## \n#\n### ###" => '<h2></h2><h1></h1><h3></h3>'
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testSetextHeadings(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "Foo *bar*\n=========\n\nFoo *bar*\n---------" => '<h1>Foo <em>bar</em></h1><h2>Foo <em>bar</em></h2>',
            "Foo *bar\nbaz*\n====" => "<h1>Foo <em>bar\nbaz</em></h1>",
            "Foo\n-------------------------\n\nFoo\n=" => '<h2>Foo</h2><h1>Foo</h1>',
            "   Foo\n---\n\n  Foo\n-----\n\n  Foo\n  ===" => '<h2>Foo</h2><h2>Foo</h2><h1>Foo</h1>',
            "    Foo\n    ---\n\n    Foo\n---" => '<pre><code>Foo' . "\n" . '---' . "\n\n" . 'Foo' . "\n" . '</code></pre><hr>',
            "Foo\n   ----      " => '<h2>Foo</h2>',
            "Foo\n    ---" => "<p>Foo\n---</p>",
            "Foo\n= =\n\nFoo\n--- -" => "<p>Foo\n= =</p><p>Foo</p><hr>",
            "Foo  \n-----" => '<h2>Foo</h2>',
            "Foo\\\n----" => '<h2>Foo\</h2>',
            "`Foo\n----\n`\n\n<a title=\"a lot\n---\nof dashes\"/>" => '<h2>`Foo</h2><p>`</p><h2>&lt;a title=&quot;a lot</h2><p>of dashes&quot;/&gt;</p>',
            "> Foo\n---" => '<blockquote><p>Foo</p></blockquote><hr>',
            "> foo\nbar\n===" => "<blockquote><p>foo\nbar\n===</p></blockquote>",
            "- Foo\n---" => "<ul><li>Foo</li></ul><hr>",
            "Foo\nBar\n---" => "<h2>Foo\nBar</h2>",
            "---\nFoo\n---\nBar\n---\nBaz" => '<hr><h2>Foo</h2><h2>Bar</h2><p>Baz</p>',
            "\n====" => '<p>====</p>',
            "---\n---" => '<hr><hr>',
            "- foo\n-----" => '<ul><li>foo</li></ul><hr>',
            "    foo\n---" => '<pre><code>foo' . "\n" . '</code></pre><hr>',
            "> foo\n-----" => '<blockquote><p>foo</p></blockquote><hr>',
            "\> foo\n------" => '<h2>&gt; foo</h2>'
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testIndentedCodeBlocks(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "    a simple\n      indented code block" => "<pre><code>a simple\n  indented code block\n</code></pre>",
            "  - foo\n\n    bar" => '<ul><li><p>foo</p><p>bar</p></li></ul>',
            "1.  foo\n\n    - bar" => '<ol><li><p>foo</p><ul><li>bar</li></ul></li></ol>',
            "    <a/>\n    *hi*\n\n    - one" => "<pre><code>&lt;a/&gt;\n*hi*\n\n- one\n</code></pre>",
            "    chunk1\n\n    chunk2\n  \n \n \n    chunk3" => "<pre><code>chunk1\n\nchunk2\n\n\n\nchunk3\n</code></pre>",
            "    chunk1\n      \n      chunk2" => "<pre><code>chunk1\n  \n  chunk2\n</code></pre>",
            "Foo\n    bar" => '<p>Foo' . "\n" . 'bar</p>',
            "    foo\nbar" => '<pre><code>foo' . "\n" . '</code></pre><p>bar</p>',
            "# Heading\n    foo\nHeading\n------\n    foo\n----" => "<h1>Heading</h1><pre><code>foo\n</code></pre><h2>Heading</h2><pre><code>foo\n</code></pre><hr>",
            "        foo\n    bar" => "<pre><code>    foo\nbar\n</code></pre>",
            "\n    \n    foo\n    " => "<pre><code>foo\n</code></pre>",
            "    foo  " => "<pre><code>foo  \n</code></pre>"
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testFencedCodeBlocks(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "```\n<\n >\n```" => "<pre><code>&lt;\n &gt;\n</code></pre>",
            "~~~\n<\n >\n~~~" => "<pre><code>&lt;\n &gt;\n</code></pre>",
            "```ruby\ndef foo(x)\n  return 3\nend\n```" => "<pre><code class=\"language-ruby\">def foo(x)\n  return 3\nend\n</code></pre>"
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testParagraphs(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "aaa\n\nbbb" => '<p>aaa</p><p>bbb</p>',
            "aaa\nbbb\n\nccc\nddd" => "<p>aaa\nbbb</p><p>ccc\nddd</p>",
            "aaa\n\n\nbbb" => '<p>aaa</p><p>bbb</p>',
            "  aaa\n bbb" => "<p>aaa\nbbb</p>",
            "aaa\n             bbb\n                                       ccc" => "<p>aaa\nbbb\nccc</p>",
            "   aaa\nbbb" => "<p>aaa\nbbb</p>",
            "    aaa\nbbb" => "<pre><code>aaa\n</code></pre><p>bbb</p>",
            "aaa     \nbbb     " => "<p>aaa<br>bbb</p>"
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testBlockquotes(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "> # Foo\n> bar\n> baz" => "<blockquote><h1>Foo</h1><p>bar\nbaz</p></blockquote>",
            "># Foo\n>bar\n> baz" => "<blockquote><h1>Foo</h1><p>bar\nbaz</p></blockquote>",
            "   > # Foo\n   > bar\n > baz" => "<blockquote><h1>Foo</h1><p>bar\nbaz</p></blockquote>",
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testListItems1(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "1.  A paragraph\n    with two lines.\n\n        indented code\n\n    > A block quote." => "<ol><li><p>A paragraph\nwith two lines.</p><pre><code>indented code\n</code></pre><blockquote><p>A block quote.</p></blockquote></li></ol>",
            "- one\n\n two" => "<ul><li>one</li></ul><p>two</p>",
            "- one\n\n  two" => "<ul><li><p>one</p><p>two</p></li></ul>",
            " -    one\n\n     two" => "<ul><li>one</li></ul><pre><code> two\n</code></pre>",
            " -    one\n\n      two" => '<ul><li><p>one</p><p>two</p></li></ul>'
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testLists1(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "- foo\n- bar\n+ baz" => '<ul><li>foo</li><li>bar</li></ul><ul><li>baz</li></ul>',
            "1. foo\n2. bar\n3) baz" => '<ol><li>foo</li><li>bar</li></ol><ol start="3"><li>baz</li></ol>',
            "Foo\n- bar\n- baz" => '<p>Foo</p><ul><li>bar</li><li>baz</li></ul>',
            "The number of windows in my house is\n14.  The number of doors is 6." => '<p>The number of windows in my house is</p><ol start="14"><li>The number of doors is 6.</li></ol>',
            "- foo\n\n- bar\n\n\n- baz" => '<ul><li><p>foo</p></li><li><p>bar</p></li></ul><ul><li>baz</li></ul>'
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }

    <<Test>>
    public async function testHtmlBlocks(Assert $assert): Awaitable<void>
    {
        $tests = Map{
            "<table>\n  <tr>\n    <td>\n           hi\n    </td>\n  </tr>\n</table>\n\nokay." => "<table>\n  <tr>\n    <td>\n           hi\n    </td>\n  </tr>\n</table><p>okay.</p>"
        };
        foreach ($tests as $in => $out) {
            $a = <axe:markdown text={$in} docParser={$this->docParser}/>;
            $assert->string((string)$a)->is($out);
        }
    }
}
