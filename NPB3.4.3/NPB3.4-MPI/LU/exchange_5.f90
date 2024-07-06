
!---------------------------------------------------------------------
!---------------------------------------------------------------------

      subroutine exchange_5(g,ibeg,ifin1)

!---------------------------------------------------------------------
!---------------------------------------------------------------------

!---------------------------------------------------------------------
!   compute the right hand side based on exact solution
!---------------------------------------------------------------------

      use lu_data
      use mpinpb

      implicit none

!---------------------------------------------------------------------
!  input parameters
!---------------------------------------------------------------------
      integer ibeg, ifin1
      double precision  g(0:isiz2+1,0:isiz3+1)

!---------------------------------------------------------------------
!  local variables
!---------------------------------------------------------------------
      integer k
      double precision  dum(isiz03)

      integer IERROR



!---------------------------------------------------------------------
!   communicate in the south and north directions
!---------------------------------------------------------------------

!---------------------------------------------------------------------
!   receive from south
!---------------------------------------------------------------------
      if (ifin1.eq.nx) then
        call MPI_IRECV( dum,  &
     &                  nz,  &
     &                  dp_type,  &
     &                  south,  &
     &                  from_s,  &
     &                  comm_solve,  &
     &                  msgid1,  &
     &                  IERROR )

        call MPI_WAIT( msgid1, STATUS, IERROR )

        do k = 1,nz
          g(nx+1,k) = dum(k)
        end do

      end if

!---------------------------------------------------------------------
!   send north
!---------------------------------------------------------------------
      if (ibeg.eq.1) then
        do k = 1,nz
          dum(k) = g(1,k)
        end do

        call MPI_SEND( dum,  &
     &                 nz,  &
     &                 dp_type,  &
     &                 north,  &
     &                 from_s,  &
     &                 comm_solve,  &
     &                 IERROR )

      end if

      return
      end     