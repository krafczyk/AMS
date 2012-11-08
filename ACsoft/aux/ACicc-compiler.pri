# "remark #181: argument is incompatible with corresponding format string conversion"
# This warning chokes on ac_producer, which should be fixed.
QMAKE_CXXFLAGS += -wd181

# "remark #383: value copied to temporary, reference to temporary used"
# Fires on foobar("test", ..) string literals, where we except const string& references.
# Seems to work fine, we should investigate - I think this is a false positive.
QMAKE_CXXFLAGS += -wd383

# "remark #444: destructor for base class "std::iterator<std::bidirectional_iterator_tag, TObject *, ptrdiff_t={long},
# const TObject **, const TObject *&>" (declared at line 110 of "/usr/include/c++/4.3/bits/stl_iterator_base_types.h") is not virtual"
# Workaround ROOT 5.27 warning /lustre/jwork/vsk10/vsk1003/Offline/root/Linux/527.icc64/include/TObjArray.h(124): 
QMAKE_CXXFLAGS += -wd444

# "remark #981: operands are evaluated in unspecified order"
# This is a false positive, suppress it.
QMAKE_CXXFLAGS += -wd981

# "remark #1418: external function definition with no prior declaration"
# This can safely be ignord according to the ICC docs.
QMAKE_CXXFLAGS += -wd1418

# "remark #1419: external declaration in primary source file"
# This can safely be ignord according to the ICC docs.
QMAKE_CXXFLAGS += -wd1419

# "remark #1720: function "AC::ChargeAndError::operator new(size_t={unsigned long}, NotNullTag, void *)" has no
# "corresponding member operator delete (to be called if an exception is thrown during initialization of an allocated object)
# This is related to WTF_MAKE_ALLOCATED and can safely be suppressed - WTFs design just uses a special new operator.
QMAKE_CXXFLAGS += -wd1720

# "remark #2259: non-pointer conversion from "double" to "Float_t={float}" may lose significant bits"
# We should seriously investigate in each of these warnings.
QMAKE_CXXFLAGS += -wd2259
