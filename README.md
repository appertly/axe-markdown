# appertly/axe-markdown

**A** ppertly **X** HP **E** xtras Markdown: Markdown to XHP in Hack/HHVM.

[![Packagist](https://img.shields.io/packagist/v/appertly/axe-markdown.svg)](https://packagist.org/packages/appertly/axe-markdown)
[![Build Status](https://travis-ci.org/appertly/axe-markdown.svg)](https://travis-ci.org/appertly/axe-markdown)

## Installation

You can install this library using Composer:

```console
$ composer require appertly/axe-markdown
```

* The master branch (version 0.x) of this project requires HHVM 3.12 and depends on [XHP](https://github.com/facebook/xhp-lib) 2.2+ and [league/commonmark](https://github.com/thephpleague/commonmark).

## Compliance

Releases of this library will conform to [Semantic Versioning](http://semver.org).

Our code is intended to comply with [PSR-1](http://www.php-fig.org/psr/psr-1/) and [PSR-2](http://www.php-fig.org/psr/psr-2/). If you find any issues related to standards compliance, please send a pull request!

## Features

Instead of using `league/commonmark` to actually *render* the HTML, we use it to parse Markdown into an Abstract Syntax Tree, then we create real _bona fide_ XHP nodes from that AST.

### Example
```hack
function getXhp(): XHPRoot
{
    $str = <<<EOD
# Hello!
This is great
* Don't you think?
EOD;
    return <axe:markdown text={$str}/>;
}
```
Calling this function will produce XHP nodes exactly like this:
```html
<h1>Hello!</h1><p>This is great</p><ul><li>Don't you think?</li></ul>
```
You can also supply your own `DocParser` in case you've registered custom extensions.
```hack
$environment = League\CommonMark\Environment::createCommonMarkEnvironment();
// customize Environment
$docParser = new League\CommonMark\DocParser($environment);
// customize DocParser
echo <axe:markdown text={$text} docParser={$docParser}/>;
```
