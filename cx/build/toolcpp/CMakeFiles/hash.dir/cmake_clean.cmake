file(REMOVE_RECURSE
  "libhash.a"
  "libhash.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/hash.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
