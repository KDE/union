# Style Definitions for the Plasma SVG Plugin

This directory contains style definitions used by the Plasma SVG input plugin to
create style rules. The original Plasma SVG based system makes a lot of hidden
assumptions and relies on things encoded in QtQuick implementations for its
final look, so we need a way to encode that information without relying on the
same code.

All the YAML files in this directory will be read and parsed by the Plasma SVG
plugin. Each file is expected to contain a mapping. The keys are ignored, but
each value is expected to be another mapping. These top-level mappings can have
the several properties, as described below.

## Selection Properties

These are used to build selectors used to determine what elements this style
rule targets. Note that when specifying multiple properties, these will be
combined into a single selector and all properties need to match for the
selector to match.

<br/>
<dl>
    <dt>type</dt>
    <dd>A string describing the type of element to target.</dd>
    <dt>id</dt>
    <dd>A string with a specific element ID to target</dd>
    <dt>state</dt>
    <dd>A string corresponding to one of several element states to target.</dd>
    <dt>hints</dt>
    <dd>A list of strings with specific hints to target.</dd>
    <dt>attributes</dt>
    <dd>A mapping with specific attributes to target.</dd>
    <dt>colorSet</dt>
    <dd>A specific color set to target.<br/>
        Note that this is both a selection property and a cascading
        property.</dd>
</dl>

## Cascading Properties

These properties are primarily used to inform about which parts of the Plasma
SVG theme should be used when the current style rule is applied to an element.
These properties are applied to any other properties within the style rule,
except for children and unless overridden.

<br/>
<dl>
    <dt>path</dt>
    <dd>The path to an SVG file within a theme. Should be relative to the
        theme root.</dd>
    <dt>element</dt>
    <dd>The ID of an element inside the SVG to use.</dd>
    <dt>prefix</dt>
    <dd>A prefix to apply to the element name. These are combined by placing a
        hyphen between them.</dd>
    <dt>colorSet</dt>
    <dd>The color set to use when rendering any images of an element.</dd>
</dl>

## Style Rule Properties

These properties all define some part of the actual styling properties to apply
when the style rule is applied. Note that when a value is mentioned here this
is expected to be a mapping that describes some kind of element property
selection function, as described below.

<br/>
<dl>
    <dt>margins</dt>
    <dd>A size definition defining the size of the margins.</dd>
    <dt>padding</dt>
    <dd>A size definition defining the size of the paddings.</dd>
    <dt>border</dt>
    <dd>A border definition defining the size and appearance of the border.</dd>
    <dt>corners</dt>
    <dd>
        A mapping containing `top-left`, `top-right`, `bottom-left` and
        `bottom-right`. Each of those is expected to contain another mapping
        containing `size` with the size of the corner in pixels and `image`
        with an image to display for the corner.
    </dd>
    <dt>background</dt>
    <dd>A mapping containing `size` with the background size in pixels and
        `image` with an image to display for the background.</dd>
    <dt>shadow</dt>
    <dd></dd>
    <dt>text</dt>
    <dd></dd>
</dl>

## Element Property Selection Functions

These are all functions that do a lookup of an element in an SVG file and then
do something with that element. They are generally used to provide the actual
values for style properties to use. They are used by creating a mapping and
specifying `property` as one of the keys, the other keys depend on which
function you want to use. Note that which element these functions target is set
by using the cascading properties specified above, which may be inherited from
a parent mapping.

<br/>
<dl>
    <dt>constant</dl>
    <dd>
        A statically defined constant value, specified with the `value` key,
        which is required. This function is an exception to the above that
        states you should create a mapping for a function, in that if you don't
        create a mapping this function is implicitly used.<br/>
        Note that this function does not actually make use of any elements in
        the SVG and instead always has a fixed value.
    </dd>
    <dt>element-size</dt>
    <dd>
        Gets and returns the size of an element in the SVG.<br/>
        Optionally, you can specify the key `invert` with a boolean value, that
        if true, inverts the size.
    </dd>
    <dt>element-width</dt>
    <dd>
        Gets and returns the width of an element. Uses `element-size` internally
        so anything that applies to that function also applies here.
    </dd>
    <dt>element-height</dt>
    <dd>
        Gets and returns the height of an element. Uses `element-size`
        internally so anything that applies to that function also applies here.
    </dd>
    <dt>element-image</dt>
    <dd>
        Renders an element to an image and returns the image. The element is
        rendered using the current `colorSet` at the size specified in the SVG
        file.
    </dd>
    <dt>element-image-blend</dt>
    <dd>
        Renders multiple elements and combines them into a single image which
        it returns. Expects a key `elements` that contains a list of strings
        with element names to render. Elements are rendered based on the order
        specified in the `elements` list, with later images blended on top of
        earlier images using alpha blending. The resulting image's size is the
        maximum of all invididual images' sizes. You can optionally specify an
        `align` key to specify alignment if images differ in size.<br/>
        Note that each element in the list of elements is prefixed by the
        cascading `prefix` property and the cascading `element` property is
        ignored.
    </dd>
</dl>

## Style Definitions

<br/>
<dl>
    <dt>Size Definition</dt>
    <dd>
        A mapping containing the keys `left`, `right`, `top` and `bottom`. Each
        key is expected to contain a size in pixels.
    </dd>
    <dt>Border Definition</dt>
    <dd>
        A mapping containing the keys `left`, `right`, `top` and `bottom`. Each
        key is expected to contain a mapping with the keys `size` and `image`.
        `size` is expected to contain a size in pixels and `image` an image
        rendered from an element in the current SVG.
    </dd>
    <dt></dt>
    <dd></dd>
</dl>

## Other Declarations

<br/>
<dl>
    <dt>Alignment</dt>
    <dd>
        Certain properties or functions require you to specify alignment. This
        is specified by a mapping containing a `horizontal` and `vertical` key.
        Valid values for `horizontal` are `left`, `center` and `right`. Valid
        values for `vertical` are `top`, `center`, `bottom`.
    </dd>
</dl>
