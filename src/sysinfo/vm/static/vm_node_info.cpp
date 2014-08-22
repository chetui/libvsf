
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<atomic>
#include<mutex>
using namespace std;

struct vm_vnode_static_info
{
	int vnode_id;
	vector<int> cpus;
	int mem_size_mb;
	int static_bind;
};

struct vm_vnode_return_structure
{
	int vnode_size;
	vector<vm_vnode_static_info> vnode_info;
};

const int BUF_SIZE=10240;

string read_cmd_line(uint32_t pid)
{
	FILE *fp;
	char proc_filename[BUF_SIZE];
	char *tmp_buf = new char[BUF_SIZE];
	char *buf=NULL;
	string ret;
	int read_size=0, total_size=0;
	sprintf(proc_filename, "/proc/%u/cmdline", pid);
	fp = fopen(proc_filename, "r");
	if (fp == NULL)
		goto error_out;
	read_size = fread(tmp_buf, 1, BUF_SIZE, fp);
	while (read_size >0)
	{
		char * tmp;
		tmp = new char[total_size + BUF_SIZE];
		memcpy(tmp, buf, total_size);
		memcpy(&tmp[total_size], tmp_buf, read_size);
		delete[] buf;
		buf = tmp;
		total_size += read_size;
		read_size = fread(tmp_buf, 1, BUF_SIZE, fp);
	}
	for (int i = 0; i < total_size-1;i++)
	if (buf[i] == 0)
		buf[i] = ' ';
	ret = buf;
	delete[] buf;
	fclose(fp);
error_out:
	delete[] tmp_buf;
	return ret;
}

inline string extract_attr(string attr_str, string name)
{
	int start = attr_str.find(name + '=') + (name + '=').size();
	int end=start;
	if (start == -1)
		return "";
	for (; end < attr_str.size() && attr_str[end] != '='; end++)
		;
	if (end != attr_str.size())
		for (; end>start&&attr_str[end] != ','; end--)
			;
		return string(attr_str.begin() + start, attr_str.begin() + end);
}

vector<int> parse_cpu_string(string cpu_str)
{
	char *buf = new char[cpu_str.size() + 1],*token;
	vector<int> ret;
	memcpy(buf, cpu_str.c_str(), cpu_str.size());
	token = strtok(buf, ",");
	while (token)
	{
		char *pos = strstr(token, "-");
		char *token2;
		if (pos)
		{
			token2 = pos + 1;
			*pos = 0;
			for (int i = atoi(token); i <= atoi(token2); i++)
				ret.push_back(i);
		}
		else
			ret.push_back(atoi(token));
		token = strtok(NULL, ",");
	}
	delete[] buf;
	return ret;
}

vm_vnode_return_structure get_vm_vnode_static_info_from_cmdline(string cmdline)
{
	stringstream cmdstream(cmdline);
	string token;
	vm_vnode_return_structure ret;
	ret.vnode_size = 0;
	while (cmdstream >> token)
	{
		if (token == "-numa" && cmdstream >> token)
		{
			vm_vnode_static_info vm_node;
			string attr_value_str;
			ret.vnode_size++;
			attr_value_str = extract_attr(token, "nodeid");
			if (attr_value_str != "")
				vm_node.vnode_id = atoi(attr_value_str.c_str());
			attr_value_str = extract_attr(token, "cpus");
			if (attr_value_str != "")
				vm_node.cpus = parse_cpu_string(attr_value_str);
			attr_value_str = extract_attr(token, "mem");
			if (attr_value_str != "")
				vm_node.mem_size_mb = atoi(attr_value_str.c_str());
			attr_value_str = extract_attr(token, "membind");
			if (attr_value_str != "")
				vm_node.static_bind = atoi(attr_value_str.c_str());
			ret.vnode_info.push_back(vm_node);
		}
	}
	return ret;
}

class vm_node_info
{
	int vnode_num;
	vector<vm_vnode_static_info> vnode_info;
	atomic<bool> init;
	mutex lock;
	vm_node_info():vnode_num(-1),init(false);
public:
	vm_node_info *get_instance();
	
	int update(uint32_t pid);
	int get_vnode_num();
}

	vm_node_info *vm_node_info::get_instance()
	{
		static vm_node_info instance;
		return *instance;
	}
	
	int vm_node_info::update(uint32_t pid)
	{
		string cmdline=read_cmd_line(pid);
		mutex.lock();
		*((vm_vnode_return_structure *) this)=get_vm_vnode_static_info_from_cmdline(cmdline);
		mutex.unlock();
		init=true;
		return 0;
	}
	
	int vm_node_info::get_vnode_num()
	{
		if(!init)
			update();
		return vnode_num;
	}
	
	vector<int> vm_node_info::get_vnode_ids()
	{
		if(!init)
			update();
		vector<int> ret;
		for(auto &vnode:vnode_info)
			ret.push_back(vnode.vnode_id);
		return ret;
	}
	
	vector<int> vm_node_info::get_vcpu_ids(int vnode_id,int vcpu_num=0)
	{
		if(!init)
			update();
		vector<int> ret;	
		for(auto &vnode:vnode_info)
			if(vnode.vnode_id==vnode_id)
			{
				ret=vnode.cpus;
				break;
			}
		if(ret.size()==0&&vcpu_num>0)
		{
			for(int i=vnode_id*(vcpu_num/vnode_num);i<(vnode_id+1)*(vcpu_num/vnode_num))]
				ret.push_back(i);
		}
		return ret;
	}
	
	
	