#ifdef __CINT__
      void Begin(TTree* tree){
              AMSEventR::Begin(tree);
              UBegin();
      };

      void ProcessFill(Int_t entry){
              AMSEventR::ProcessFill(entry);
              if (!UProcessCut()) return;
              UProcessFill();
      };

      void Terminate(){
              UTerminate();
              AMSEventR::Terminate();
      };
#endif
