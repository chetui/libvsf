#include"vm_node_info.cpp"

template<class _Ty>
ostream & operator<<(ostream& out,const vector<_Ty> v)
{
    for(auto &elem:v)
        out<<elem<<" ";
    return out;
}

int main(int argc,char **argv)
{
    vm_node_info *v=vm_node_info::get_instance(30579);
    v->update();
    vector<int> ret;
    int vnode_num=v->get_vnode_num();
    cout<<"vnode_num="<<vnode_num<<endl;
    ret=v->get_vnode_ids();
    cout<<"node_ids:"<<ret<<endl;
    for(int i=0;i<vnode_num;i++)
    {
        ret=v->get_vcpu_ids(i);
        cout<<"vnode "<<i<<"vcpu:"<<ret<<endl;
    }
    return 0;
}
