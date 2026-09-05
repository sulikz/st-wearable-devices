function normal(shader, t_base, t_second, t_detail)
	shader:begin	("wd_nixie", "wd_led")
		: fog		(false)
		: zb		(true,false)
		: blend		(true,blend.one,blend.invsrcalpha)
		: sorting	(1, true)

	shader:dx10texture	("s_base",	t_base)
	shader:dx10sampler	("smp_base")
end
