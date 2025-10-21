file(REMOVE_RECURSE
  "PiWeather/qml/main.qml"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/piweather_tooling.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
