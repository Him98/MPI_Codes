#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;
#define pb push_back
#define mp make_pair
#define f first
#define s second
#define sc(n) scanf("%d",&n)
#define scl(n) scanf("%lld",&n)
#define pr(n) printf("%d",n)
#define prl(n) printf("%lld",n)
#define nl printf("\n")
#define INF 10000000
#define fast_io ios_base::sync_with_stdio(false);cin.tie(NULL)
ll pow1(ll a, ll b)
{
	ll ans=1;
	while(b)
	{
		if(b&1)
		{
			ans=ans*a;
		}
		a=a*a;
		b=b>>1;
	}
	return ans;
}
int main()
{
	int i,j,n,m,u,v,src,w;
	scanf("%d %d %d",&n,&m,&src);
    int gr[n][n];
    for(i=0;i<n;i++)
        for(j=0;j<n;j++)
            gr[i][j] = INF;
    for(i=0;i<m;i++)
	{
		scanf("%d %d %d",&u,&v,&w);
		if(u == src)
		{
			u = 0;
			if(v == 0)
				v = src;
		}
		else if(v == src)
		{
			v = 0;
			if(u == 0)
				u = src;
		}
		else if(u == 0)
		{
			u = src;
			if(v == src)
				v = 0;
		}
		else if(v == 0)
		{
			v = src;
			if(u == src)
				u = 0;
		}
		gr[u][v] = w;
		gr[v][u] = w;
	}
    cout<<n<<" "<<m<<" "<<src<<endl;
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			cout<<gr[i][j]<<" ";
	return 0;
}