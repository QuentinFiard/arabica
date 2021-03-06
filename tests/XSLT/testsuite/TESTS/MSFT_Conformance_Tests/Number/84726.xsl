<?xml version="1.0"?> 
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output encoding="UTF-16"/>

<!-- XSLT - Numbering - format= "&#x4e01;" (Far East Decimal) & = "&#x58f1;" (Far East Ideographic) - letter-value="alphabetic" | "traditional" -->
<!-- Created : Khalil Jabrane -->
<!-- Date: 04/12/2000 -->

<xsl:template match="/">  
	<TABLE>  
		<xsl:for-each select="//現的輸入法">
			<TR><TD>  
				<TD>
					<!-- Far East Decimal -->
					<xsl:number format="&#x4e01;" letter-value="traditional"/> *					
					<xsl:value-of select = "text()"/>
				</TD>
		
				<TD>|-----|</TD>

				<TD>
					<!-- Far East Ideographic -->
					<xsl:number format="&#x58f1;" letter-value="alphabetic"/> *					
					<xsl:value-of select = "text()"/>
				</TD>

			</TD></TR>
		</xsl:for-each>  
	</TABLE>
</xsl:template>  

</xsl:stylesheet>  

