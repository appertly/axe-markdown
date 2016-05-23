<?hh
/**
 * Hack typechecker definitions for the league/commonmark package.
 */
namespace League\CommonMark;

use League\CommonMark\Block\Element\Document;

class DocParser
{
    public function __construct(protected Environment $environment)
    {
    }

    public function getEnvironment(): Environment
    {
        return $this->environment;
    }

    public function parse(?string $input): Document
    {
        return new Document();
    }
}

class Environment
{
    public function __construct(array $config = [])
    {
    }

    public static function createCommonMarkEnvironment(): Environment
    {
        return new Environment([]);
    }
}

namespace League\CommonMark\Node;

abstract class Node
{
    public function children(): array<Node>
    {
        return [];
    }
}

namespace League\CommonMark\Block\Element;

abstract class AbstractBlock extends Node
{
    public function getData<T>(string $key, ?T $default = null): ?T
    {
        return $default;
    }
}

class BlockQuote extends AbstractBlock
{
}

class Document extends AbstractBlock
{
}

class FencedCode extends AbstractBlock
{
    public function getInfoWords(): array<string>
    {
        return [];
    }
}

interface InlineContainer
{
    public function getStringContent(): string;
}

class Heading extends AbstractBlock implements InlineContainer
{
    public function getLevel(): int
    {
        return 1;
    }
}

class HtmlBlock extends AbstractBlock implements InlineContainer
{
}

class IndentedCode extends AbstractBlock
{
}

class ListBlock extends AbstractBlock
{
}

class ListItem extends AbstractBlock
{
}

class Paragraph extends AbstractBlock implements InlineContainer
{
}

class ThematicBreak extends AbstractBlock
{
}

namespace League\CommonMark\Inline\Element;

use League\CommonMark\Node\Node;

abstract class AbstractInline extends Node
{
    public function getData<T>(string $key, ?T $default = null): ?T
    {
        return $default;
    }
}

abstract class AbstractInlineContainer extends AbstractInline
{
}

class AbstractStringContainer extends AbstractInline
{
    public function getContent(): string
    {
        return '';
    }
}

class Code extends AbstractStringContainer
{
}

class Emphasis extends AbstractInlineContainer
{
}

class HtmlInline extends AbstractStringContainer
{
}

class Image extends AbstractWebResource
{
}

class Link extends AbstractWebResource
{
}

class Newline extends AbstractInline
{
    const HARDBREAK = 0;
    const SOFTBREAK = 1;

    public function getType(): int
    {
        return 0;
    }
}

class Strong extends AbstractInlineContainer
{
}

class Text extends AbstractStringContainer
{
}
