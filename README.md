static-regexp
=============

Static regular expression using c++11 templates.

Usage
-----

	make
	./build/native/translate [ns]
 
Write your regexp to stdin and copy translated output from stdout. And insert it as template parameter for StaticRegExp class in your code.

Example
-------

	^[a-z]+://[a-z][a-z0-9]+(\.[a-z][a-z0-9]+)*/

will turn into:

	Seq<Begin,Plus<CSet<'a','z'>>,Str<0x3a,0x2f,0x2f>,CSet<'a','z'>,Plus<CSet<0x30,0x39,'a','z'>, Star<Seq<Chr<0x2e>,CSet<'a','z'>,Plus<CSet<0x30,0x39,'a','z'>>>, Chr<0x2f>>>>

