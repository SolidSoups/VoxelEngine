#include "EditorIO.h"

EditorIO& EditorIO::Get(){
  static EditorIO io;
  return io;
}
