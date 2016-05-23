<?hh // strict
/**
 * Axe
 *
 * @copyright 2015-2016 Appertly contributors
 * @license   MIT
 */

use League\CommonMark\Block\Element\AbstractBlock;
use League\CommonMark\Block\Element\BlockQuote;
use League\CommonMark\Block\Element\FencedCode;
use League\CommonMark\Block\Element\Heading;
use League\CommonMark\Block\Element\HtmlBlock;
use League\CommonMark\Block\Element\IndentedCode;
use League\CommonMark\Block\Element\ListBlock;
use League\CommonMark\Block\Element\ListItem;
use League\CommonMark\Block\Element\Paragraph;
use League\CommonMark\Block\Element\ThematicBreak;
use League\CommonMark\DocParser;
use League\CommonMark\Environment;
use League\CommonMark\Inline\Element\AbstractInline;
use League\CommonMark\Inline\Element\Code;
use League\CommonMark\Inline\Element\Emphasis;
use League\CommonMark\Inline\Element\HtmlInline;
use League\CommonMark\Inline\Element\Image;
use League\CommonMark\Inline\Element\Link;
use League\CommonMark\Inline\Element\Newline;
use League\CommonMark\Inline\Element\Strong;
use League\CommonMark\Inline\Element\Text;
use League\CommonMark\Node\Node;

/**
 * Turns Markdown syntax into XHP nodes.
 */
class :axe:markdown extends :x:element
{
    category %flow;
    children empty;
    attribute ?Stringish text,
        ?DocParser docParser;

    private static ImmMap<string,mixed> $emptyMap = ImmMap{};

    protected function render(): XHPRoot
    {
        $text = $this->:text;
        if ($text === null) {
            return <x:frag/>;
        }
        $text = (string)$text;
        if ($text === '') {
            return <x:frag/>;
        }
        $parser = $this->:docParser;
        if ($parser === null) {
            $environment = Environment::createCommonMarkEnvironment();
            $parser = new DocParser($environment);
        }
        $document = $parser->parse($text);
        $blocks = $this->renderBlocks($document->children());
        return $blocks === null ? <x:frag/> : <x:frag>{$blocks}</x:frag>;
    }

    private function renderInlines(Traversable<AbstractInline> $inlines): Vector<XHPChild>
    {
        $results = Vector{};
        foreach ($inlines as $inline) {
            $results[] = $this->renderInline($inline);
        }
        return $results;
    }

    private function renderInline(AbstractInline $inline): XHPChild
    {
        if ($inline instanceof Code) {

            $b = <code>{$inline->getContent()}</code>;
            $b->setAttributes($inline->getData('attributes', []) ?? self::$emptyMap);
            return $b;

        } elseif ($inline instanceof Emphasis) {

            /* HH_IGNORE_ERROR[4110]: OK */
            $b = <em>{$this->renderInlines($inline->children())}</em>;
            $b->setAttributes($inline->getData('attributes', []) ?? self::$emptyMap);
            return $b;

        } elseif ($inline instanceof HtmlInline) {

            return new AxeUnsafeMarkdown($inline->getContent());

        } elseif ($inline instanceof Image) {

            $b = <img/>;
            $b->setAttributes($inline->getData('attributes', []) ?? self::$emptyMap);
            $b->setAttribute('src', $inline->getUrl());
            $alt = implode("", $this->renderInlines($inline->children()));
            $alt = preg_replace('/\<[^>]*alt="([^"]*)"[^>]*\>/', '$1', $alt);
            $b->setAttribute('alt', preg_replace('/\<[^>]*\>/', '', $alt));
            $t = (string)($inline->data['title'] ?? '');
            if (strlen(trim($t)) > 0) {
                $b->setAttribute('title', $inline->data['title']);
            }
            return $b;

        } elseif ($inline instanceof Link) {

            $b = <a>{$this->renderInlines($inline->children())}</a>;
            $b->setAttributes($inline->getData('attributes', []) ?? self::$emptyMap);
            $b->setAttribute('href', $inline->getUrl());
            $t = (string)($inline->data['title'] ?? '');
            if (strlen(trim($t)) > 0) {
                $b->setAttribute('title', $inline->data['title']);
            }
            return $b;

        } elseif ($inline instanceof Newline) {

            if ($inline->getType() === Newline::HARDBREAK) {
                return <br/>;
            } else {
                return <x:frag>{"\n"}</x:frag>;
            }

        } elseif ($inline instanceof Strong) {

            /* HH_IGNORE_ERROR[4110]: OK */
            $b = <strong>{$this->renderInlines($inline->children())}</strong>;
            $b->setAttributes($inline->getData('attributes', []) ?? self::$emptyMap);
            return $b;

        } elseif ($inline instanceof Text) {

            return <x:frag>{$inline->getContent()}</x:frag>;

        }

        return <x:frag/>;
    }

    private function renderBlocks(Traversable<AbstractBlock> $blocks, bool $inTightList = false): Vector<XHPChild>
    {
        $results = Vector{};
        foreach ($blocks as $block) {
            $results[] = $this->renderBlock($block, $inTightList);
        }
        return $results;
    }

    private function renderBlock(AbstractBlock $block, bool $inTightList = false): XHPChild
    {
        if ($block instanceof BlockQuote) {

            $filling = $this->renderBlocks($block->children());
            $b = <blockquote>{$filling}</blockquote>;
            $b->setAttributes($block->getData('attributes', []) ?? self::$emptyMap);
            return $b;

        } elseif ($block instanceof FencedCode) {

            $infoWords = $block->getInfoWords();
            $b = <code>{$block->getStringContent()}</code>;
            $b->setAttributes($block->getData('attributes', []) ?? self::$emptyMap);
            if (count($infoWords) !== 0 && strlen($infoWords[0]) !== 0) {
                $b->addClass('language-' . $infoWords[0]);
            }
            return <pre>{$b}</pre>;

        } elseif ($block instanceof Heading) {

            $kids = $this->renderInlines($block->children());
            switch ($block->getLevel()) {
                case 1:
                    $b = <h1>{$kids}</h1>;
                    break;
                case 2:
                    $b = <h2>{$kids}</h2>;
                    break;
                case 3:
                    $b = <h3>{$kids}</h3>;
                    break;
                case 4:
                    $b = <h4>{$kids}</h4>;
                    break;
                case 5:
                    $b = <h5>{$kids}</h5>;
                    break;
                case 6:
                    $b = <h6>{$kids}</h6>;
                    break;
            }
            $b->setAttributes($block->getData('attributes', []) ?? self::$emptyMap);
            return $b;

        } elseif ($block instanceof HtmlBlock) {

            return new AxeUnsafeMarkdown($block->getStringContent());

        } elseif ($block instanceof IndentedCode) {

            $b = <code>{$block->getStringContent()}</code>;
            $b->setAttributes($block->getData('attributes', []) ?? self::$emptyMap);
            return <pre>{$b}</pre>;

        } elseif ($block instanceof ListBlock) {

            $listData = $block->getListData();
            $b = $listData->type === ListBlock::TYPE_UNORDERED ? <ul/> : <ol/>;
            $b->appendChild(
                $this->renderBlocks($block->children(), $block->isTight())
            );
            $b->setAttributes($block->getData('attributes', []) ?? self::$emptyMap);
            if ($listData->start !== null && $listData->start !== 1) {
                $b->setAttribute('start', (int) $listData->start);
            }
            return $b;

        } elseif ($block instanceof ListItem) {

            $contents = $this->renderBlocks($block->children(), $inTightList);
            $b = <li>{$contents}</li>;
            $b->setAttributes($block->getData('attributes', []) ?? self::$emptyMap);
            return $b;

        } elseif ($block instanceof Paragraph) {

            if ($inTightList) {
                return <x:frag>{$this->renderInlines($block->children())}</x:frag>;
            } else {
                $b = <p>{$this->renderInlines($block->children())}</p>;
                $b->setAttributes($block->getData('attributes', []) ?? self::$emptyMap);
                return $b;
            }

        } elseif ($block instanceof ThematicBreak) {

            $b = <hr/>;
            $b->setAttributes($block->getData('attributes', []) ?? self::$emptyMap);
            return $b;

        }

        return <x:frag/>;
    }
}
