

























#pragma ident "%Z%%M% %I% %E% SMI"



















#include <strings.h>
#include <stdlib.h>
#include <assert.h>

#include <dt_impl.h>
#include <dt_program.h>
#include <dt_provider.h>
#include <dt_pcb.h>






void
dt_pcb_push(dtrace_hdl_t *dtp, dt_pcb_t *pcb)
{








if (yypcb != NULL) {
assert(yypcb->pcb_hdl == dtp);
assert(yypcb->pcb_yystate == YYS_DONE);
}

bzero(pcb, sizeof (dt_pcb_t));

dt_scope_create(&pcb->pcb_dstack);
dt_idstack_push(&pcb->pcb_globals, dtp->dt_globals);
dt_irlist_create(&pcb->pcb_ir);

pcb->pcb_hdl = dtp;
pcb->pcb_prev = dtp->dt_pcb;

dtp->dt_pcb = pcb;
dtp->dt_gen++;

yyinit(pcb);
}

static int
dt_pcb_pop_ident(dt_idhash_t *dhp, dt_ident_t *idp, void *arg)
{
dtrace_hdl_t *dtp = arg;

if (idp->di_gen == dtp->dt_gen)
dt_idhash_delete(dhp, idp);

return (0);
}






void
dt_pcb_pop(dtrace_hdl_t *dtp, int err)
{
dt_pcb_t *pcb = yypcb;
uint_t i;

assert(pcb != NULL);
assert(pcb == dtp->dt_pcb);

while (pcb->pcb_dstack.ds_next != NULL)
(void) dt_scope_pop();

dt_scope_destroy(&pcb->pcb_dstack);
dt_irlist_destroy(&pcb->pcb_ir);

dt_node_link_free(&pcb->pcb_list);
dt_node_link_free(&pcb->pcb_hold);

if (err != 0) {
dt_xlator_t *dxp, *nxp;
dt_provider_t *pvp, *nvp;

if (pcb->pcb_prog != NULL)
dt_program_destroy(dtp, pcb->pcb_prog);
if (pcb->pcb_stmt != NULL)
dtrace_stmt_destroy(dtp, pcb->pcb_stmt);
if (pcb->pcb_ecbdesc != NULL)
dt_ecbdesc_release(dtp, pcb->pcb_ecbdesc);

for (dxp = dt_list_next(&dtp->dt_xlators); dxp; dxp = nxp) {
nxp = dt_list_next(dxp);
if (dxp->dx_gen == dtp->dt_gen)
dt_xlator_destroy(dtp, dxp);
}

for (pvp = dt_list_next(&dtp->dt_provlist); pvp; pvp = nvp) {
nvp = dt_list_next(pvp);
if (pvp->pv_gen == dtp->dt_gen)
dt_provider_destroy(dtp, pvp);
}

(void) dt_idhash_iter(dtp->dt_aggs, dt_pcb_pop_ident, dtp);
dt_idhash_update(dtp->dt_aggs);

(void) dt_idhash_iter(dtp->dt_globals, dt_pcb_pop_ident, dtp);
dt_idhash_update(dtp->dt_globals);

(void) dt_idhash_iter(dtp->dt_tls, dt_pcb_pop_ident, dtp);
dt_idhash_update(dtp->dt_tls);

(void) ctf_discard(dtp->dt_cdefs->dm_ctfp);
(void) ctf_discard(dtp->dt_ddefs->dm_ctfp);
}

if (pcb->pcb_pragmas != NULL)
dt_idhash_destroy(pcb->pcb_pragmas);
if (pcb->pcb_locals != NULL)
dt_idhash_destroy(pcb->pcb_locals);
if (pcb->pcb_idents != NULL)
dt_idhash_destroy(pcb->pcb_idents);
if (pcb->pcb_inttab != NULL)
dt_inttab_destroy(pcb->pcb_inttab);
if (pcb->pcb_strtab != NULL)
dt_strtab_destroy(pcb->pcb_strtab);
if (pcb->pcb_regs != NULL)
dt_regset_destroy(pcb->pcb_regs);

for (i = 0; i < pcb->pcb_asxreflen; i++)
dt_free(dtp, pcb->pcb_asxrefs[i]);

dt_free(dtp, pcb->pcb_asxrefs);
dt_difo_free(dtp, pcb->pcb_difo);

free(pcb->pcb_filetag);
free(pcb->pcb_sflagv);

dtp->dt_pcb = pcb->pcb_prev;
bzero(pcb, sizeof (dt_pcb_t));
yyinit(dtp->dt_pcb);
}
