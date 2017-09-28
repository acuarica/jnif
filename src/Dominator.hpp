#ifndef JNIF_DOMINATOR_HPP
#define JNIF_DOMINATOR_HPP

namespace jnif {

    struct Dominator : std::map<BasicBlock*, std::set<BasicBlock*> > {

        Dominator(ControlFlowGraph& cfg) {
            for (BasicBlock* bb : cfg) {
                (*this)[bb].insert(cfg.basicBlocks.begin(), cfg.basicBlocks.end());
            }

            bool changed = true;
            while (changed) {
                changed = false;

                for (BasicBlock* bb : cfg) {
                    std::set<BasicBlock*> ns;
                    for (BasicBlock* p : bb->targets) {
                        if (ns.empty()) {
                            ns = (*this)[p];
                        } else {
                            for (BasicBlock* bbp : ns) {
                                if ((*this)[p].count(bbp) == 0) {
                                    ns.erase(bbp);
                                }
                            }
                        }
                    }
                    ns.insert(bb);

                    if (ns != (*this)[bb]) {
                        changed = true;
                        (*this)[bb] = ns;
                    }
                }
            }
        }
    };

}

#endif
