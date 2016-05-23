<?hh // strict
/**
 * Axe
 *
 * @copyright 2015-2016 Appertly contributors
 * @license   MIT
 */

/**
 * Container for unsafe nodes.
 */
class AxeUnsafeMarkdown implements XHPUnsafeRenderable
{
    /**
     * Creates an AxeUnsafeMarkdown.
     *
     * @param $content - The HTML
     */
    public function __construct(private string $content)
    {
    }

    /**
     * Gets the HTML.
     *
     * @return The string content
     */
    public function toHTMLString(): string
    {
        return $this->content;
    }
}
