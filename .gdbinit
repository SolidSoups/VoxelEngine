file build/game_engine

define build
  shell ./build_compile.sh
  if $_shell_exitcode == 0
    run 
  end
end


