 
# IDHash

## Overview

The Important Difference Hash (IDHash), created by NaikilonD, is a variant of the Difference Hash algorithm (DHash). Both compute a "fuzzy" hash of an image that can be used to detect near-duplicate images. Images that are near-duplicates have a smaller hash distance between them (on average) than non-duplicates.

This implementation uses VIPS for the image processing steps.

## Abstract

The effectiveness of IDHash as a near-duplicate / non-duplicate detector was evaluated.
x1 =
x2 = 
x3 = 
x4 =

## Results

Cool graph with gnuplot, like the one is CVAA.

## Notes on shrinking

For shrinking, vips_thumbnail is used. You'd think you would want to do something even lower level like shrink, or even just use the --linear option to shrink in RGB instead of sRGB. But tests with JPEG images indicate that using vips_thumbnail, which by default uses libjpeg's shrink-on-load functionality, leads to stellar near-duplicate detection. One side effect of JPEG compression is that the idhash for an image need not be the same every time. Concurrency also probably plays a role in this.

This peculiarity occasionaly manifests itself as false positives (non-duplicates classified as near-duplicates) or false negatives (near-duplicates classified as non-duplicates). But on the whole, the fuzziness of the image shrinking process is just part of the algorithm, which is a black box anyway.

This does mean that the algorithm is tied to the vips version, but recomputes are cheap, since the algorithm is fast. 

In the future, we'll try different shrinking algorithms and look at the metrics to see if they offer improvements in speed or efficiency.

## Operations on images in AWS S3

If you have the AWS C++ SDK, you can stream an image from an AWS S3 URL, through the IDHash VIPS pipeline, and back to the S3 URL. If you have an AWS Lambda function that has the AWS C++ Runtime, you can do that in a Lambda function, and possibly even hook into AWS S3 Batch Operations to automate some things for you. Amazon!

## Notes on concurrency

This implementation computes the x- and y-direction hashes on two separate threads. Uses pthread, but I'll write another version that uses Jens Gustet-style threads (which are provided by GCC on some platforms - looking at you cyber operators).

## Notes on false positives / false negatives

Image this embarassing situation: your organization's new near-duplicate detection program suggests with a high degree of certainty that two different images of two different people are actually near-duplicates. How do you respond to criticisms of the developer's methods and intent? What factors played a role in the algorithm's decision? What does that say about the developer? About the organization as a whole?

The best way to explain why this error occurred is with diagnostics.

By zooming the image hashes to a viewable size, say 500x500, you can produce a viewable image version of an image's hash, either by showing the difference hash and importance array side-by-side, or applying the importance array (which doesn't fully capture information about the distance between hashes).

The picture representations of the image hashes and the simplicity of the algorithm make it easy to see how the algorithm failed to classify a pair of images correctly. You can see how edges line up for matching images, or see how one image has many random edges in one corner causing it to fail to match. It shows the geometry behind the problem, and protects the integrity of your organization.

## Overview

Both IDHash and classic DHash algorithms start by shrinking the original image down to 8x8 (or 9x9 in some variations of DHash). The 8x8 x and y discrete derivatives of this small image are then computed. The sign of the derivative image pixels are used computed the x and y difference hashes, which are 64-bit bit-arrays that point out the direction of the gradient at each pixel.

IDHash takes this a step further and computes x and y importance arrays, which are 64-bit bit-arrays that identify which differences where less than the median. 

When computing differences between idhashes, the x and y components are considered separately, and the two results are summed. A component is computed by taking the union of the important pixels, intersecting that with the symmetric difference of the pixels where the gradient is positive, and then counting the pixels.

In the language of bitwise operators:

idhash = {dx, dy, ix, iy}
          ^   ^   ^   ^
          |   |   |   |_ y-direction importance array
          |   |   |_____ x-direction importance array
          |   |_________ y-direction difference hash
          |_____________ x-direction difference hash

distance = sum( (d1x ^ d2x) & (i1x | i2x) ) + sum( ( d1y ^ d2y) & (i1y | i2y) )

## Algorithm Details

1. Shrink to 8x8 (no, really), grayscale, and extract a single channel. This implementation uses VIPS for these image processing steps. In particular, it uses vips_thumbnail.

2. Compute 8x8 difference arrays for the x and y directions. To get the final difference, subtract the first pixel value from the last in that row column. I casually refer to this as "wrapping".

3. Compute 8x8 difference hashes for the x and y directions. Each entry is 1 ifthe difference was positive or 0, and 0 if the difference is (nonzero) negative. 

3. Compute 8x8 importance arrays for the x and y directions. Each entry is 1 if the difference was greater than or equal to the median difference, and 0 if the difference was less than the median difference.

4. The idhash is formed by viewing each component 64-bit bit-array as a 64-bit integer. These four 64-bit integers are then stored in an array (or concatenated if you have some kind a "big integer" class handy.) in the following order:

<dx> <dy> <ix> <iy>

5. The distance between

# Misc. Notes

##The x and y difference arrays

These will contain differences of adjacent
pixels, starting at (0, 0) and moving right and down, subtracting the
current pixel value from the next pixel value. The last difference
wraps around - it is taken between the first and last pixels.


##The x and y difference hashes (dx and dy)

Each element is 1 or 0. It is 1 if the
corresponding difference is positive, and 0 if it is negative.
Implemented as bit arrays, represented simply by a 64-bit integer.


##The x and y importances (ix and iy)

Each element is 1 or 0. It is 0 if the 
corresponding difference is less than the median difference, and 1 
otherwise.

##The histograms

Since we're in 8-bit RGB we can use a histogram of frequency of integer 
differences in the range [0, 255] to find the median using a bin sort. The 
histogram we use is special in that its bins are bit arrays for the indices. 
Later the bits in each bin are summed until the median is found. There is a histogram for each direction. Each is utilized only by the corresponding thread for that direction. 

## Multi-threading

One for the x-direction, and one for the y-direction. Uses pthread.

## Test coverage

Not exhaustive yet, but off the ground. The next step is to create a sample of many possible inputs for each of the test functions. For some of them, it is possible to be exhaustive - the inputs can be enumerated in acceptable time.

## Using "big int" data types to simplify math expressions


If your platform allows for 128 integer types, or you're using Boost or Java or something and you have access to a "big integer" class, you can simply concatenate the x- and y-components of the difference hash and importance for each image:

Say the pseudocode function concat() creates a 2n-bit integer from 2 n-bit
integers by sticking them together. Just say the left arg is the big end, 
but it doesn't actually matter.

d1 = concat(d1x, d1y),    d2 = concat(d2x, d2y),
i1 = concat(i1x, i1y),    i2 = concat(i2x, i2y)

Then you can do the same bit operations as we've done on 64-bit integers,
but on a half as many 128-bit integers instead (syntactically only probably - your CPU architecture probably doesn't natively support a 128-bit integer type. if it does, cool!).

dist = sum( (d1 ^ d2) & (i1 ^ i2) )

And it looks nicer, which is cool.

# Notes on JPEG compression

Q: Is there only one mathematically correct decoding of a JPEG?

A: No. From the JPEG FAQ:

"Another important aspect of JPEG is that decoders can trade off decoding speed against image quality, by using fast but inaccurate approximations to the required calculations. Some viewers obtain remarkable speedups this way."

The same JPEG decoder applied to the same image twice doesn't have to produce the same exact bit stream either, since e.g. slight timing differences in concurrent operations could produce different results. 
