Highly granular thread-safe array
==============================

A highly granular thread-safe array I spent a chunk of yesterday creating.  Always bugged me somewhat that mutexes lack granularity as far as arrays and matrices are concerned.  A fairly simple explanatory test harness/example is attached.

If you're new to template-based programming and are wondering why all of the code is in the .h file, read http://www.parashift.com/c++-faq-lite/templates-defn-vs-decl.html.  In C++11 you can use extern template; I assumed most people are still using C++03 thus didn't utilise this.

By Tim Bashford - 11/05/13.  Use as you see fit.