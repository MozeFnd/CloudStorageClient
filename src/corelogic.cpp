#include "corelogic.h"
#include "getnamedialog.h"

CoreLogic::CoreLogic(std::shared_ptr<KVStore> kvstore, std::shared_ptr<Communicator> communicator, std::shared_ptr<PathTree> path_tree)
    :kvstore_(kvstore)
    ,communicator_(communicator)
    ,path_tree_(path_tree){

    // kvstore_->store("computer_name", "");
    std::string c_name = kvstore_->read("computer_name");
    if (c_name.empty()) {
        GetNameDialog* getname = new GetNameDialog();
        kvstore_->store("computer_name", getname->getName().toStdString());
    }

}
