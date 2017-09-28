#ifndef JNIF_DOMINATOR_HPP
#define JNIF_DOMINATOR_HPP

namespace jnif {

    template <class TDir>
    struct BaseDominator : std::map<BasicBlock*, std::set<BasicBlock*> > {

        BaseDominator(ControlFlowGraph& cfg) {
            for (BasicBlock* bb : cfg) {
                (*this)[bb].insert(cfg.basicBlocks.begin(), cfg.basicBlocks.end());
            }

            bool changed = true;
            while (changed) {
                changed = false;

                for (BasicBlock* bb : cfg) {
                    std::set<BasicBlock*> ns;
                    for (BasicBlock* p : TDir::dir(bb)) {
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


        template <class T>
        friend std::ostream& operator<<(std::ostream& os, BaseDominator<T> ds) {
            for (auto d : ds) {
                os << d.first->name << ": ";
                for (auto dp : d.second) {
                    os << "  " << dp->name << " ";
                }
                os << std::endl;
            }

            return os;
        }
    };

    struct Dominator : BaseDominator<Dominator> {
        using BaseDominator<Dominator>::BaseDominator;
        static std::vector<BasicBlock*>& dir(BasicBlock* bb) { return bb->ins; }
    };

    struct PostDominator : BaseDominator<PostDominator> {
        using BaseDominator<PostDominator>::BaseDominator;
        static std::vector<BasicBlock*>& dir(BasicBlock* bb) { return bb->targets; }
    };
}

#endif
