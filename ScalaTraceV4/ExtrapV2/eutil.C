#include "ecommon.h"
#include "eutil.h"
#include "umpi_mpi_ops.h"

/*vector<LCDSET>	lcd_vec ;
vector<LCDSET>::iterator	lcd_it ;*/

int	lcd_val[3] ;
int	max_lcd_idx = 3 ;

/*int	islcd(int	t , int	lcd_val)
{
	if ( (t % lcd_val) == 0 ) return 1 ;
	else return -1 ;
}




int	isArranged()
{
	int	prev = 0 ;
	for ( int i = 0 ; i < max_lcd_idx ; i ++ ) {
		if ( lcd_val[i] < prev ) return 0 ;
		prev = lcd_val[i] ;
	}
	return 1 ;
}


void	_decompose(int	num , int	token)
{
	if ( token == 1 ) {	// Add to the vecotrs
		if ( num == 1 ) return ;

		lcd_val[max_lcd_idx - token] = num ;

		if ( !isArranged() ) return ;
		
		LCDSET	ls ;
		memcpy(&ls.lcd_val[0], lcd_val , 3 * sizeof(int)) ;
		ls.ltype = LCDT_3D ;
		lcd_vec.push_back(ls) ;
	} else {
		for ( int i = 2 ; (i <= num / 2) && (i <= num / i ); i ++ ) 
			if ( islcd(num , i ) > 0 )	{ 
				lcd_val[max_lcd_idx - token] = i ;
				_decompose( num / i, token - 1) ;
			}
	}
}

int	decompose(int	*arr, int	count)
{
	vector<vector<LCDSET	> >	pLcd_vec ;
	
	max_lcd_idx = 3 ;
	for ( int j = 0 ; j < count ; j ++ ) {
		_decompose(arr[j] , max_lcd_idx ) ;

		if ( !lcd_vec.size() ) { // lcd failure break ;
			break ;
		}
		pLcd_vec.push_back(lcd_vec) ;		

		for ( int i = 0 ; i < lcd_vec.size(); i ++ ) {
			LCDSET	t = lcd_vec.at(i) ;
			printf("%d :%d %d %d %d\n", arr[j], t.lcd_val[0],t.lcd_val[1],t.lcd_val[2], t.ltype) ;
		}
		printf("\n") ;
		lcd_vec.clear() ;
	}

	for ( int i = 0 ; i < pLcd_vec.size() ; i ++ ) {

	}
	return 1;
}*/

int is_async_pol (int op)
{
  switch (op)
  {
    case umpi_MPI_Ibsend :case umpi_MPI_Irecv :
    case umpi_MPI_Irsend :case umpi_MPI_Isend :case umpi_MPI_Issend :
      return 1;
    default:
      return 0;
  }
}

/*
int	is_sync(int	op)
{
  switch (op)
  {
    case umpi_MPI_Send :case umpi_MPI_Recv :case umpi_MPI_Rsend :case umpi_MPI_Ssend :
		case umpi_MPI_Sendrecv:
      return 1;
    default:
      return 0;
  }
}*/

int is_collective_pol (int op)
{
  switch (op)
  {
    case umpi_MPI_Allgather :case umpi_MPI_Allgatherv :case umpi_MPI_Allreduce :
    case umpi_MPI_Alltoall : case umpi_MPI_Alltoallv : //case umpi_MPI_Barrier :
    //case umpi_MPI_Finalize :
    case umpi_MPI_Gather : case umpi_MPI_Gatherv :
    case umpi_MPI_Reduce :case umpi_MPI_Reduce_scatter :case umpi_MPI_Scatter :
    case umpi_MPI_Scatterv :
      return 1;
    default:
      return 0;
  }
}

int  is_communicate(int	op)
{
	if ( is_async_pol(op) ) return 1 ;
	if ( is_collective_pol(op) ) return 1 ;

	return 0 ;
}


int	is_sendop(int	op){
	switch (op){
		case umpi_MPI_Ssend:
		case umpi_MPI_Send:
		case umpi_MPI_Bsend:
		case umpi_MPI_Ibsend:
		case umpi_MPI_Irsend:
		case umpi_MPI_Isend:
		case umpi_MPI_Issend:
		case umpi_MPI_Rsend:
		case umpi_MPI_Sendrecv:
			return 1;
		default:
			return 0;
	}
}

void CAL_DIM(const vector<int> *rlist, vector<DIM> *xyz, int *mdepth){
	vector<int> list;
	for(unsigned i=1; i<(*rlist).size(); i++){
		if(((*rlist)[i]<0 && i!=1) || i==(*rlist).size()-1){
			DIM temp;
			if(i==(*rlist).size()-1)
				list.push_back((*rlist)[i]);
			temp.single_list = list;
			temp.dimension = -list[0];
			temp.start_rank = list[1];
			temp.xstride=temp.ystride=temp.zstride = 0;
			temp.xdepth=temp.ydepth=temp.zdepth = 0;
			temp.xmask=temp.ymask=temp.zmask = 0;
			*mdepth = MAX(*mdepth, temp.dimension);
			(*xyz).push_back(temp);
			list.clear();
			list.push_back((*rlist)[i]);	
		} else
			list.push_back((*rlist)[i]);
	}
}

