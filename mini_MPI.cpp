#include<iostream>
#include<vector>
#include <bits/stdc++.h>
#include<algorithm>
#include <iomanip>
#include <mpi.h>

/// user plays first
using namespace std;
struct node;
node* createNode(vector<int>,int);
vector<vector<int>> generateStates(vector<int>,int);
node* creatTree(node*,int);
void print_tree(node*);
void values_of_leafs(node*);
vector<int> minimax_new(node*,int );
node* find_node(vector<int> ,node*,int);
int minimax(node* ,int );
int level=0;
int total__levels;
int player_id;
int myrank,nproc;
vector<int>pile{3,4,5};

vector<node*> leafs;
node*root=createNode(pile,0);//playerA
struct node{
vector<int>state;
//int player;
int value;
int level_of_node;
vector<node* >child;

};
node* createNode(vector<int>tempPile,int tempvalue){
node* newNode=new node;
newNode->state=tempPile;
//newNode->player=tempvalue;
newNode->value=0;
return newNode;

}
vector<vector<int>> generateStates(vector<int>curr_state,int tempPlayer){
vector<vector<int>> states;
for(int i=0;i<curr_state.size();i++){

    if(curr_state[i]>0){
        for(int j=1;j<=3;j++){
            vector<int>temp;
            for(int h=0;h<curr_state.size();h++){
               // cout<<curr_state[h]<<" ";
                temp.push_back(curr_state[h]);
            }
            temp[i]-=j;
            if(temp[i]<0) break;

            for(int h=0;h<curr_state.size();h++){
                //cout<<temp[h]<<" ";
            }
            states.push_back(temp);


        }
    }
    }
return states;
}
node* creatTree(node* aroot,int player){
vector<vector<int>> child_state=generateStates(aroot->state,player);

for(int i=0;i<child_state.size();i++){
    node* child=createNode(child_state[i],1-player);
    aroot->child.push_back(child);
    aroot->level_of_node=level;
    creatTree(child,1-player);
}

return root;

}
void values_of_leafs(node* root){
int val=0;
    if (root==NULL)
        return;
    queue<node *> q;  // Create a queue
    q.push(root); // Enqueue root
level=0;
    while (!q.empty())
    {
        int n = q.size();
        //cout<<"level:"<<level<<endl;
         if(level%2==0) {
            val=1;}
        else {val=0;}
        while (n > 0)
        {
            node * p = q.front();
            int tempSum=0;
            p->level_of_node=level;

            for(int i=0;i<p->state.size();i++){
              tempSum+=p->state[i];
            //cout << p->state[i]<< " ";
            }
            if(tempSum==0){
                p->value=val;
                leafs.push_back(p);
            }
            //answer.push_back(p);
             q.pop();
            //cout<<"value:"<<p->value<<"  ";
            // Enqueue all children of the dequeued item
            for (int i=0; i<p->child.size(); i++)
                q.push(p->child[i]);
            n--;
        }
    level++;
        cout << endl; // Print new line between two levels

    }
//reverse(begin(answer), end(answer));
total__levels=level;
}
void print_tree(node* root){
if(root->child.size()!=0){

    for(int i=0;i<root->child.size();i++){
        print_tree(root->child[i]);
    }
}
cout<<"level"<<root->level_of_node<<endl;
cout<<root->state[0]<<root->state[1]<<root->state[2];
cout<<"value :"<<root->value<<endl;
}
node* node_found=new node;
node* pre_node=root;
vector<int> minimax_new(vector<int>p,int isMax){
int maxi[2],mini,a,maxiloc[2];
maxiloc[0]=0;
node* aroot=new node;
aroot=find_node(p,pre_node,0);
pre_node=node_found;
for(int j=myrank;j<(aroot->child.size());j+=nproc){
            //cout<<"rank"<<myrank<<"is working on node"<<aroot->child[j]->state[0]<<aroot->child[j]->state[1]<<endl;
                a=minimax(aroot->child[j],false);

                //cout<<"value returend "<<endl;
                //MPI_send(&a,1,MPI_INT,0,0,MPI_COMM_WORLD);
           if(a>maxiloc[0]){

            maxiloc[0]=a;
            maxiloc[1]=j;
            //cout<<"local max "<<maxiloc[0]<<"local index"<<maxiloc[1]<<endl;
           }

        }


         MPI_Allreduce(&maxiloc,&maxi,1,MPI_2INT,MPI_MAXLOC,MPI_COMM_WORLD);

        //the problem here is that each process has it's own copy so the rank who make change on the maxi value only he can see that change and all other
        //process see the maxi unchanged
        // asolustion coould be by sending the value of all the nodes to the master and he calculate the max of them
        //MPI_Barrier(MPI_COMM_WORLD);
        //reduce will return the max of all the children node and the location of that max
        //so the next move for the computer will be to go to that max node
        MPI_Barrier(MPI_COMM_WORLD);
  //      if(myrank==0){
            //cout<<"maxi value"<<maxi[0]<<"maxi node "<<maxi[1]<<endl;
            //aroot->value=maxi;
        return aroot->child[maxi[1]]->state;
//        }


}
int minimax(node* aaroot,int isMax){
    if(aaroot->child.size()==0) {
         return aaroot->value;}
if(isMax){
          int maxi=0;
        for(int j=0;j<(aaroot->child.size());j++){
                int a=minimax(aaroot->child[j],false);
           if(a>maxi){
            maxi=a;
           }
        }
            //cout<<"assign"<<maxi<< "to node"<<aaroot->state[0]<<aaroot->state[1]<<"in level"<<aaroot->level_of_node<<endl;
            aaroot->value=maxi;
            return maxi;
}
else{

 int mini=1;
//cout<<"taking min of child of node"<<aroot->state[0]<<aroot->state[1]<<aroot->state[2]<<"level "<<aroot->level_of_node<<endl;
        for(int h=0;h<(aaroot->child.size());h++){
                int a=minimax(aaroot->child[h],true);
           if(a<mini){
            mini=a;
           }
        }
//cout<<"assign"<<mini<< "to node"<<aaroot->state[0]<<aaroot->state[1]<<"in level"<<aaroot->level_of_node<<endl;
aaroot->value=mini;
   return mini;



}


   }
node* find_node(vector<int> apile,node*aroot,int x){

 queue<node *> q;  // Create a queue
    q.push(root);
    while(!q.empty()){
        node* p=q.front();
        if (p->state==apile){
                //cout<<"found node"<<endl;
            return p;
        }
        q.pop();
        for (int i=0; i<p->child.size(); i++){
                q.push(p->child[i]);}
    }

}
void remove_from_piles(int index,int number,int id ){
while(pile[index]<number){
    cout<<"number of stick to remove is larger than the pile enter again :"<<endl;
    cin>>index>>number;
}
pile[index]=pile[index]-number;
player_id=1-player_id;
//cout<<"removed from pile by player"<<endl;
}

int main(){

double start,finish,T;
creatTree(root,0);
values_of_leafs(root);
int moves[2];
MPI_Init(0,0);
MPI_Comm_size(MPI_COMM_WORLD, &nproc);
MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
player_id=0;///computer play first
//int moves[2]; ///first item for index of pile the second is the number of coins to be take
if(myrank==0){
cout<<"let's start the game who take last coin lose"<<endl;
cout<<"pile is "<<pile[0]<<pile[1]<<pile[2]<<endl;
cout<<"computer plays first"<<endl;
}
while((pile[0]+pile[1]+pile[2])!=0){ ///game not finished yet
    if(player_id==1){
    if(myrank==0){
        cout<<"enter the pile index and the number if sticks to remove"<<endl;
        cin>>moves[0]>>moves[1];}

        //remove_from_piles(moves[0],moves[1],player_id);
        //MPI_Bcast(moves,2,MPI_INT,0,MPI_COMM_WORLD);
        //}
        MPI_Bcast(moves,2,MPI_INT,0,MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        //cout<<"move are "<<moves[0]<<moves[1]<<"in process "<<myrank<<endl;

        remove_from_piles(moves[0],moves[1],player_id);
    }
    else{

       MPI_Barrier(MPI_COMM_WORLD);
       start=MPI_Wtime();
       //cout<<"pile enterd to fucntion "<<pile[0]<<pile[1]<<pile[2]<<endl;
       pile=minimax_new(pile,true);
       finish=MPI_Wtime();
       //MPI_Barrier(MPI_COMM_WORLD);
        T=finish-start;
        if(myrank==0){
        cout<<"computer turn "<<endl;
       // cout<<"parallel time execution in sec:" <<T<< endl;
        }
        player_id=1;
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if(myrank==0){
            cout<<"pile after move is "<<pile[0]<<pile[1]<<pile[2]<<endl;}
    //MPI_Barrier(MPI_COMM_WORLD);
}
if(myrank==0){
cout<<"game is done and ";
if(player_id==0){ //becuse at the end of remove_from_pile function it toggles the id
    cout<<"computer wins"<<endl;
}
else{
    cout<<"user wins"<<endl;
}
}
MPI_Finalize();
return 0;
}


