#ifndef __TNIRPS_VAR_MAP_H__
#define __TNIRPS_VAR_MAP_H__

class VarMap {
public:
   VarMap (const char* str) { // receives a comma-separated list of variables
      Array<char> buf;
      BufferScanner s(str);
      s.skipSpace();
      int idc = 0;
      while (!s.isEOF()) {
         char c = 0;
         s.readWord(buf, ",");
         buf.pop();
         while (isspace(buf.top()))
            buf.pop();
         if (buf.size() == 0)
            throw Exception("Empty identifiers not allowed");
         for (int i = 0; i < buf.size(); ++i)
            if (!isalpha(buf[i]) && (i == 0 || !isdigit(buf[i])))
               throw Exception("Variable identifier must start with a letter and contain only letters and digits");
         buf.push(0);
         if (!s.isEOF())
            s.readChar();
         s.skipSpace();
         int id = idc++;
         name2id.insert(buf.ptr(), id);
         id2name.insert(id).copy(buf);
      }
   }

   bool isName (const char* name) {
      return name2id.find(name);
   }

   int id (const char* name) {
      return name2id.at(name);
   }

   const char* name (int id) {
      return id2name.at(id).ptr();
   }

   void print(Output& output) {
      for (int i = id2name.begin(); i < id2name.end(); i = id2name.next(i))
         output.printf(i == id2name.begin() ? "%s" : ", %s", id2name.value(i).ptr());
   }

private:
   RedBlackStringMap<int> name2id;
   RedBlackObjMap<int, Array<char> > id2name;
};

#endif /* __TNIRPS_VAR_MAP_H__ */

